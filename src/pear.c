#include <appling.h>
#include <assert.h>
#include <fx.h>
#include <js.h>
#include <log.h>
#include <path.h>
#include <stdint.h>
#include <string.h>
#include <uv.h>

#if defined(APPLING_OS_LINUX)
#include <libgen.h>
#include <unistd.h>
#endif

#include "../include/pear.h"

static uv_thread_t pear__thread;
static uv_process_t pear__process;

static fx_window_t *pear__window;

static appling_link_t pear__app_link;
static appling_lock_t pear__lock;
static const char *pear__path;
static appling_resolve_t pear__resolve;
static appling_bootstrap_t pear__bootstrap;
static uint64_t pear__bootstrap_start;

static appling_platform_t pear__platform = {
  .key = {0x6b, 0x83, 0x74, 0xf1, 0xc0, 0x80, 0x9e, 0xd2, 0x3c, 0xfc, 0x37, 0x1e, 0x87, 0x89, 0x6c, 0x8d, 0x3b, 0xb5, 0x93, 0xf2, 0x45, 0x1d, 0x4d, 0x8d, 0xe8, 0x95, 0xd6, 0x28, 0x94, 0x18, 0x18, 0xdc},
  .length = 8844,
};

static appling_app_t pear__app = {0};
static const char *pear__app_name;

static void
pear__on_close(fx_t *fx, void *data) {
  int err;

  err = fx_close_window(pear__window);
  assert(err == 0);

#if PEAR_RESTART_AFTER_BOOTSTRAP
  err = appling_open(&pear__app, NULL);
  assert(err == 0);
#endif
}

static void
pear__on_unlock_boostrap(appling_lock_t *req, int status) {
  int err;

  assert(status == 0);

  uint64_t elapsed = uv_hrtime() - pear__bootstrap_start;

  elapsed /= 1e6;

  if (elapsed < 5000) uv_sleep(5000 - elapsed);

  err = fx_dispatch(pear__on_close, NULL);
  assert(err == 0);
}

static void
pear__on_bootstrap(appling_bootstrap_t *req, int status) {
  int err;

  if (status != 0) log_fatal("%s", req->error);

  err = appling_unlock(req->loop, &pear__lock, pear__on_unlock_boostrap);
  assert(err == 0);
}

static void
pear__on_thread(void *data) {
  int err;

  uv_loop_t loop;
  err = uv_loop_init(&loop);
  assert(err == 0);

  js_platform_t *js;
  err = js_create_platform(&loop, NULL, &js);
  assert(err == 0);

  pear__bootstrap_start = uv_hrtime();

  err = appling_bootstrap(&loop, js, &pear__bootstrap, pear__platform.key, pear__app_link, pear__path, pear__on_bootstrap);
  assert(err == 0);

  err = uv_run(&loop, UV_RUN_DEFAULT);
  assert(err == 0);

  err = js_destroy_platform(js);
  assert(err == 0);

  err = uv_run(&loop, UV_RUN_DEFAULT);
  assert(err == 0);

  err = uv_loop_close(&loop);
  assert(err == 0);
}

static void
pear__on_launch(fx_t *fx) {
  int err;

  err = uv_thread_create(&pear__thread, pear__on_thread, NULL);
  assert(err == 0);

  fx_image_t *image;
  err = fx_image_init(fx, 0, 0, 400, 400, &image);
  assert(err == 0);

  appling_path_t image_path;
  size_t image_path_len = sizeof(appling_path_t);

#if defined(APPLING_OS_LINUX)
  char flatpak_path[256];
  snprintf(flatpak_path, sizeof(flatpak_path), "%s%s%s", "../share/", basename(pear__app.path), "/splash.png");
#endif

  err = path_join(
    (const char *[]) {
      pear__app.path,
#if defined(APPLING_OS_LINUX)
      access("/.flatpak-info", F_OK) == 0 ? flatpak_path : "../../../splash.png",
#elif defined(APPLING_OS_DARWIN)
      "../../Resources/splash.png",
#elif defined(APPLING_OS_WIN32)
      "../splash.png",
#else
#error Unsupported operating system
#endif
      NULL,
    },
    image_path,
    &image_path_len,
    path_behavior_system
  );
  assert(err == 0);

  err = fx_image_load_file(image, image_path, image_path_len);
  assert(err == 0);

  fx_view_t *view;
  err = fx_view_init(fx, 0, 0, 400, 400, &view);
  assert(err == 0);

  err = fx_set_child((fx_node_t *) view, (fx_node_t *) image, 0);
  assert(err == 0);

  fx_screen_t *screen;
  err = fx_get_main_screen(fx, &screen);
  assert(err == 0);

  float width, height;
  err = fx_get_screen_bounds(screen, NULL, NULL, &width, &height);
  assert(err == 0);

  err = fx_screen_release(screen);
  assert(err == 0);

  err = fx_window_init(fx, view, (width - 400) / 2, (height - 400) / 2, 400, 400, fx_window_no_frame, &pear__window);
  assert(err == 0);

  err = fx_set_window_title(pear__window, "Installing app", -1);
  assert(err == 0);

  err = fx_activate_window(pear__window);
  assert(err == 0);
}

static void
pear__on_unlock_launch(appling_lock_t *req, int status) {
  int err;

  assert(status == 0);

  err = appling_launch(&pear__platform, &pear__app, &pear__app_link, pear__app_name);
  assert(err == 0);
}

static void
pear__on_resolve(appling_resolve_t *req, int status) {
  int err;

  if (status == 0) {
    err = appling_unlock(req->loop, &pear__lock, pear__on_unlock_launch);
  } else {
    fx_t *fx;
    err = fx_init(req->loop, &fx);
    assert(err == 0);

    err = fx_run(fx, pear__on_launch, NULL);
    assert(err == 0);

    err = uv_thread_join(&pear__thread);
    assert(err == 0);
  }

  assert(err == 0);
}

static void
pear__on_lock(appling_lock_t *req, int status) {
  int err;

  assert(status == 0);

  err = appling_resolve(req->loop, &pear__resolve, pear__path, &pear__platform, pear__on_resolve);
  assert(err == 0);
}

int
pear_launch(int argc, char *argv[], pear_id_t id, const char *name, const char *link) {
  int err;

  pear__app_name = name;

  argv = uv_setup_args(argc, argv);

  err = log_open(name, 0);
  assert(err == 0);

  size_t path_len = sizeof(appling_path_t);

  err = uv_exepath(pear__app.path, &path_len);
  assert(err == 0);

  memcpy(&pear__app.id, id, sizeof(appling_id_t));

  if (link != NULL) {
    err = appling_parse(link, &pear__app_link);
    assert(err == 0);
  } else if (argc > 1) {
    err = appling_parse(argv[1], &pear__app_link);
    assert(err == 0);
  } else {
    memcpy(&pear__app_link.id, pear__app.id, sizeof(appling_id_t));
  }

  pear__path = NULL; // Default platform directory

#if defined(APPLING_OS_LINUX)
  if (getenv("SNAP_USER_COMMON") != NULL) {
    appling_path_t snap_path;
    size_t snap_path_len = sizeof(appling_path_t);

    err = path_join(
      (const char *[]) {
        getenv("SNAP_USER_COMMON"),
        "pear",
        NULL
      },
      snap_path,
      &snap_path_len,
      path_behavior_system
    );
    assert(err == 0);

    pear__path = strdup(snap_path);
  }
#endif

  err = appling_lock(uv_default_loop(), &pear__lock, pear__path, pear__on_lock);
  assert(err == 0);

  err = uv_run(uv_default_loop(), UV_RUN_DEFAULT);
  assert(err == 0);

  err = uv_loop_close(uv_default_loop());
  assert(err == 0);

  return 0;
}
