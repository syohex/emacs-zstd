/*
  Copyright (C) 2017 by Syohei YOSHIDA

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
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <emacs-module.h>

#include <zstd.h>

int plugin_is_GPL_compatible;

static char*
retrieve_string(emacs_env *env, emacs_value str, ptrdiff_t *size)
{
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

static emacs_value
Fzstd_compress(emacs_env *env, ptrdiff_t nargs, emacs_value args[], void *data)
{
	ptrdiff_t size;
	char *str = retrieve_string(env, args[0], &size);

	size_t bound = ZSTD_compressBound(size);
	char *compressed = (char*)malloc(bound);
	if (compressed == NULL) {
		return env->intern(env, "nil");
	}

	int level = (int)env->extract_integer(env, args[1]);

	size_t ret = ZSTD_compress(compressed, bound + 1, str, size-1, level);
	free(str);

	if (ZSTD_isError(ret)) {
		free(compressed);
		return env->intern(env, "nil");
	}
	compressed[ret] = '\0';

	emacs_value retval = env->make_string(env, compressed, ret);
	free(compressed);
	return retval;
}

static emacs_value
Fzstd_decompress(emacs_env *env, ptrdiff_t nargs, emacs_value args[], void *data)
{
	ptrdiff_t size;
	char *compressed = retrieve_string(env, args[0], &size);

	size_t orig_len = ZSTD_getDecompressedSize(compressed, size);
	if (orig_len == 0) {
		return env->intern(env, "nil");
	}

	char *orig = (char*)malloc(orig_len);
	if (orig == NULL) {
		return env->intern(env, "nil");
	}

	size_t ret = ZSTD_decompress(orig, orig_len + 1, compressed, size-1);
	if (ZSTD_isError(ret)) {
		free(orig);
		return env->intern(env, "nil");
	}

	orig[ret] = '\0';

	emacs_value retval = env->make_string(env, orig, ret);
	free(orig);
	return retval;
}

static void
bind_function(emacs_env *env, const char *name, emacs_value Sfun)
{
	emacs_value Qfset = env->intern(env, "fset");
	emacs_value Qsym = env->intern(env, name);
	emacs_value args[] = { Qsym, Sfun };

	env->funcall(env, Qfset, 2, args);
}

static void
provide(emacs_env *env, const char *feature)
{
	emacs_value Qfeat = env->intern(env, feature);
	emacs_value Qprovide = env->intern (env, "provide");
	emacs_value args[] = { Qfeat };

	env->funcall(env, Qprovide, 1, args);
}

int
emacs_module_init(struct emacs_runtime *ert)
{
	emacs_env *env = ert->get_environment(ert);

#define DEFUN(lsym, csym, amin, amax, doc, data) \
	bind_function (env, lsym, env->make_function(env, amin, amax, csym, doc, data))

	DEFUN("zstd-core-compress", Fzstd_compress, 2, 2, NULL, NULL);
	DEFUN("zstd-core-decompress", Fzstd_decompress, 1, 1, NULL, NULL);

#undef DEFUN

	provide(env, "zstd-core");
	return 0;
}

/*
  Local Variables:
  c-basic-offset: 8
  indent-tabs-mode: t
  End:
*/
