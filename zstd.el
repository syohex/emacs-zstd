;;; zstd.el --- zstd binding of Emacs Lisp -*- lexical-binding: t; -*-

;; Copyright (C) 2020 by Shohei YOSHIDA

;; Author: Shohei YOSHIDA <syohex@gmail.com>
;; URL: https://github.com/syohex/emacs-zstd
;; Version: 0.01
;; Package-Requires: ((emacs "26.3"))

;; This program is free software; you can redistribute it and/or modify
;; it under the terms of the GNU General Public License as published by
;; the Free Software Foundation, either version 3 of the License, or
;; (at your option) any later version.

;; This program is distributed in the hope that it will be useful,
;; but WITHOUT ANY WARRANTY; without even the implied warranty of
;; MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
;; GNU General Public License for more details.

;; You should have received a copy of the GNU General Public License
;; along with this program.  If not, see <http://www.gnu.org/licenses/>.

;;; Commentary:

;;; Code:

(require 'zstd-core)

;;;###autoload
(defun zstd-compress (input &optional level)
  (unless (stringp input)
    (user-error "Compress function support only string"))
  (let ((level (if (not level)
                   22
                 (prog1 level
                   (let ((max-level (zstd-core-max-compression-level)))
                     (when (> level max-level)
                       (user-error "Invalid level. 0 <= level <= %d" max-level)))))))
    (zstd-core-compress input level)))

(defun zstd-decompress (vec)
  (zstd-core-decompress vec))

(provide 'zstd)

;;; zstd.el ends here
