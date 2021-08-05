@echo off
SETLOCAL ENABLEDELAYEDEXPANSION

rem 需要加密的内容存放的目录
set _enc_dir_=zencrypt

if not "%1"=="" set _enc_dir_=%1

rem 加密完成后的图片文件夹
set _done_dir=done

if not "%2"=="" set _enc_dir_=%2

rem 用到加密的图片原图
set _pic_img_=pic

rem 分包大小（单位MB）
set _file_size_=5

rem 加密密码
set _password_=123456

del /Q done\*.*
del /Q tmp\*.*

mkdir %_enc_dir_%

.\7z.exe a -mx9 -md64m -mfb=273 -slp -ssw -v%_file_size_%m -p%_password_% "tmp/tmp" %_enc_dir_%
.\FuckFileMonitor.exe encrypt %_pic_img_% "tmp" %_done_dir%