@echo off
SETLOCAL ENABLEDELAYEDEXPANSION

rem 需要解密后内容存放的目录
set _dec_dir_=zdecrypt

rem 需要解密的内容
set _done_dir=done

rem 伪装的图片目录,需要和加密一样
set _pic_img_=pic

rem 设置加密密码
set _password_=123456

del /Q tmp\*.*
.\FuckFileMonitor.exe decrypt %_pic_img_% %_done_dir% tmp
.\7z.exe x -aoa -p%_password_% -o%_dec_dir_% tmp/decrypt.7z.*
