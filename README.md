# FuckFileMonitor
FuckFileMonitor 将文件加密伪装成图片,可以通过IM工具发送图片来pass内网文件监控.(其实就是蛋疼了一下写出来的东西)

原理：
1.通过7z加密分卷压缩
2.从pic文件夹获取图片文件,将图片文件和分卷压缩包合成一个文件


加密：
1.将需要加密的东西扔进zencrypt文件夹
2.双击zencrypt.bat
3.生成加密后的文件在done文件夹


解密：
1.将加密后的文件放在done文件夹
2.双击zdecrypt.bat
3.解密后的文件会在zdecrypt文件夹