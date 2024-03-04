#include <appling.h>
#include <assert.h>
#include <fx.h>
#include <js.h>
#include <log.h>
#include <path.h>
#include <string.h>
#include <uv.h>

#include "../include/pear.h"

static uv_thread_t thread;
static uv_process_t process;

static fx_window_t *window;

static appling_link_t link;
static appling_lock_t lock;
static appling_resolve_t resolve;
static appling_bootstrap_t bootstrap;

static appling_platform_t platform = {
  .dkey = {0x6b, 0x83, 0x74, 0xf1, 0xc0, 0x80, 0x9e, 0xd2, 0x3c, 0xfc, 0x37, 0x1e, 0x87, 0x89, 0x6c, 0x8d, 0x3b, 0xb5, 0x93, 0xf2, 0x45, 0x1d, 0x4d, 0x8d, 0xe8, 0x95, 0xd6, 0x28, 0x94, 0x18, 0x18, 0xdc},
};

static appling_app_t app = {0};

static void
on_close (fx_t *fx, void *data) {
  int err;

  err = fx_close_window(window);
  assert(err == 0);

  err = appling_open(&app, NULL);
  assert(err == 0);
}

static void
on_unlock_boostrap (appling_lock_t *req, int status) {
  int err;

  assert(status == 0);

  err = fx_dispatch(on_close, NULL);
  assert(err == 0);
}

static void
on_bootstrap (appling_bootstrap_t *req, int status) {
  int err;

  assert(status == 0);

  err = appling_unlock(req->loop, &lock, on_unlock_boostrap);
  assert(err == 0);
}

static void
on_thread (void *data) {
  int err;

  uv_loop_t loop;
  err = uv_loop_init(&loop);
  assert(err == 0);

  js_platform_t *js;
  err = js_create_platform(&loop, NULL, &js);
  assert(err == 0);

  err = appling_bootstrap(&loop, js, &bootstrap, platform.dkey, NULL, on_bootstrap);
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
on_launch (fx_t *fx) {
  int err;

  err = uv_thread_create(&thread, on_thread, NULL);
  assert(err == 0);

  fx_image_t *image;
  err = fx_image_init(fx, 0, 0, 400, 400, &image);
  assert(err == 0);

  appling_path_t image_path;
  size_t image_path_len = sizeof(appling_path_t);

  err = path_join(
    (const char *[]) {
      app.path,
#if defined(APPLING_OS_LINUX)
        "../../../splash.png",
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

  err = fx_window_init(fx, view, (width - 400) / 2, (height - 400) / 2, 400, 400, fx_window_no_frame, &window);
  assert(err == 0);

  err = fx_set_window_title(window, "Installing app", -1);
  assert(err == 0);

#if defined(APPLING_OS_WIN32)
  err = fx_set_window_icon(window, "icon.ico", -1);
  assert(err == 0);
#endif

  err = fx_activate_window(window);
  assert(err == 0);
}

static void
on_unlock_launch (appling_lock_t *req, int status) {
  int err;

  assert(status == 0);

  err = appling_launch(&platform, &app, &link);
  assert(err == 0);
}

static void
on_resolve (appling_resolve_t *req, int status) {
  int err;

  if (status == 0) {
    err = appling_unlock(req->loop, &lock, on_unlock_launch);
  } else {
    fx_t *fx;
    err = fx_init(req->loop, &fx);
    assert(err == 0);

    err = fx_run(fx, on_launch, NULL);
    assert(err == 0);

    err = uv_thread_join(&thread);
    assert(err == 0);
  }

  assert(err == 0);
}

static void
on_lock (appling_lock_t *req, int status) {
  int err;

  assert(status == 0);

  err = appling_resolve(req->loop, &resolve, NULL, &platform, on_resolve);
  assert(err == 0);
}

int
pear_launch (int argc, char *argv[], pear_key_t key, const char *name) {
  int err;

  argv = uv_setup_args(argc, argv);

  err = log_open(name, 0);
  assert(err == 0);

  size_t path_len = sizeof(appling_path_t);

  err = uv_exepath(app.path, &path_len);
  assert(err == 0);

  memcpy(&app.key, key, sizeof(appling_key_t));

  if (argc > 1) {
    err = appling_parse(argv[1], &link);
    assert(err == 0);
  } else {
    memcpy(&link.key, app.key, sizeof(appling_key_t));
  }

  err = appling_lock(uv_default_loop(), &lock, NULL, on_lock);
  assert(err == 0);

  err = uv_run(uv_default_loop(), UV_RUN_DEFAULT);
  assert(err == 0);

  err = uv_loop_close(uv_default_loop());
  assert(err == 0);

  return 0;
}
