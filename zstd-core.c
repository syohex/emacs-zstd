/*
  Copyright (C) 2020 by Shohei YOSHIDA

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#define _DEFAULT_SOURCE
#include <emacs-module.h>

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <zstd.h>

int plugin_is_GPL_compatible;

static char *retrieve_string(emacs_env *env, emacs_value str, ptrdiff_t *size) {
    *size = 0;

    env->copy_string_contents(env, str, NULL, size);
    char *p = malloc(*size);
    if (p == NULL) {
        *size = 0;
        return NULL;
    }
    env->copy_string_contents(env, str, p, size);

    return p;
}

static emacs_value Fzstd_compress(emacs_env *env, ptrdiff_t nargs, emacs_value args[], void *data) {
    ptrdiff_t src_size;
    uint8_t *src = (uint8_t *)retrieve_string(env, args[0], &src_size);
    src_size -= 1; // ignore null character

    intptr_t level = env->extract_integer(env, args[1]);
    size_t bound_size = ZSTD_compressBound((size_t)src_size);
    uint8_t *dest = (uint8_t *)malloc(bound_size);
    if (dest == NULL) {
        free(src);
        return env->intern(env, "nil");
    }

    size_t compressed_size = ZSTD_compress(dest, bound_size, (void *)src, src_size, (int)level);
    free(src);
    if (ZSTD_isError(compressed_size)) {
        return env->intern(env, "nil");
    }

    emacs_value Fmake_vector = env->intern(env, "make-vector");
    emacs_value func_args[2] = {
        env->make_integer(env, compressed_size),
        env->make_integer(env, 0),
    };
    emacs_value dest_vec = env->funcall(env, Fmake_vector, 2, &func_args[0]);

    for (size_t i = 0; i < compressed_size; ++i) {
        env->vec_set(env, dest_vec, i, env->make_integer(env, dest[i]));
    }

    free(dest);
    return dest_vec;
}

emacs_value Fzstd_decompress(emacs_env *env, ptrdiff_t nargs, emacs_value args[], void *data) {
    emacs_value vec = args[0];
    ptrdiff_t vec_size = env->vec_size(env, vec);
    uint8_t *src = (uint8_t *)malloc(vec_size);
    if (src == NULL) {
        return env->intern(env, "nil");
    }

    for (size_t i = 0; i < (size_t)vec_size; ++i) {
        src[i] = (uint8_t)env->extract_integer(env, env->vec_get(env, vec, i));
    }

    size_t dest_size = ZSTD_getDecompressedSize(src, vec_size);
    uint8_t *dest = (uint8_t *)malloc(dest_size);
    if (dest == NULL) {
        free(src);
        return env->intern(env, "nil");
    }

    size_t decompress_size = ZSTD_decompress(dest, dest_size, src, (size_t)vec_size);
    free(src);
    if (ZSTD_isError(decompress_size)) {
        free(dest);
        return env->intern(env, "nil");
    }

    emacs_value Fmake_vector = env->intern(env, "make-vector");
    emacs_value func_args[2] = {
        env->make_integer(env, decompress_size),
        env->make_integer(env, 0),
    };
    emacs_value dest_vec = env->funcall(env, Fmake_vector, 2, &func_args[0]);

    for (size_t i = 0; i < decompress_size; ++i) {
        env->vec_set(env, dest_vec, i, env->make_integer(env, dest[i]));
    }

    free(dest);
    return dest_vec;
}

static emacs_value Fzstd_max_clevel(emacs_env *env, ptrdiff_t nargs, emacs_value args[], void *data) {
    return env->make_integer(env, ZSTD_maxCLevel());
}

static void bind_function(emacs_env *env, const char *name, emacs_value Sfun) {
    emacs_value Qfset = env->intern(env, "fset");
    emacs_value Qsym = env->intern(env, name);
    emacs_value args[] = {Qsym, Sfun};

    env->funcall(env, Qfset, 2, args);
}

static void provide(emacs_env *env, const char *feature) {
    emacs_value Qfeat = env->intern(env, feature);
    emacs_value Qprovide = env->intern(env, "provide");
    emacs_value args[] = {Qfeat};

    env->funcall(env, Qprovide, 1, args);
}

int emacs_module_init(struct emacs_runtime *ert) {
    emacs_env *env = ert->get_environment(ert);

#define DEFUN(lsym, csym, amin, amax, doc, data) bind_function(env, lsym, env->make_function(env, amin, amax, csym, doc, data))

    DEFUN("zstd-core-compress", Fzstd_compress, 2, 2, NULL, NULL);
    DEFUN("zstd-core-decompress", Fzstd_decompress, 1, 1, NULL, NULL);
    DEFUN("zstd-core-max-compression-level", Fzstd_max_clevel, 0, 0, NULL, NULL);
#undef DEFUN

    provide(env, "zstd-core");
    return 0;
}
