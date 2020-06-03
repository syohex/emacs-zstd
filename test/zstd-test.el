;;; zstd-test.el --- zstd binding test -*- lexical-binding: t; -*-

;; Copyright (C) 2020 by Shohei YOSHIDA

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

(require 'zstd)

(ert-deftest compress-string ()
  "Test zstd-compress"
  (let* ((input (with-current-buffer (find-file-noselect "zstd.el")
                  (buffer-substring-no-properties (point-min) (point-max))))
         (ret (zstd-compress input)))
    (should (null (not ret)))
    ;; maybe compressed
    (should (> (length (string-to-vector input)) (length ret)))))

(ert-deftest compress-and-decompress ()
  "Test zstd-compress and zsd-decompress"
  (let* ((input (with-current-buffer (find-file-noselect "zstd.el")
                  (buffer-substring-no-properties (point-min) (point-max))))
         (ret (zstd-compress input)))
    (should (string= input (concat (zstd-decompress ret))))))

(provide 'zstd-test)

;;; zstd-test.el ends here
