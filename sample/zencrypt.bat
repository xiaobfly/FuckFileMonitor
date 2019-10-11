@echo off
SETLOCAL ENABLEDELAYEDEXPANSION

rem 需要加密的目录
set _enc_dir_=zencrypt

if not "%1"=="" set _enc_dir_=%1

rem 生成加密后的目录
set _done_dir=done

if not "%2"=="" set _enc_dir_=%2

rem 伪装的图片目录(记得png图片)
set _pic_img_=pic

rem 设置分包单个文件大小,单位MB
set _file_size_=5

rem 设置加密密码
set _password_=123456

del /Q done\*.*
del /Q tmp\*.*

.\7z.exe a -mx9 -md64m -mfb=273 -slp -ssw -v%_file_size_%m -p%_password_% "tmp/tmp" %_enc_dir_%
.\FuckFileMonitor.exe encrypt %_pic_img_% "tmp" %_done_dir%