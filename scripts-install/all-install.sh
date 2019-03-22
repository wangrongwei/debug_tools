#!/bin/bash

# 安装ubuntu下相关软件的脚本

# 第一步：修改root密码
echo "输入设置密码："
sudo passwd root


##########################################
# 开始安装软件
##########################################

#安装unity-tweak-tool
echo "正在安装unity-tweak-tool"
sudo apt install unity-tweak-tool

#安装截图工具：shutter
echo "正在安装shutter"
sudo apt install shutter


#安装zsh
echo "正在安装zsh"
sudo apt install zsh
#修改默认终端为zsh
chsh -s /bin/zsh
#安装oh-my-zsh
sh -c "$(wget https://raw.githubusercontent.com/robbyrussell/oh-my-zsh/master/tools/install.sh -o -)"
#修改.zshrc中ZSH_THEME = "agnoster"
#使用agnoster主题需要安装相关字体
git clone https://github.com/powerline/fonts.git --depth=1
cd fonts && ./install.sh



















#添加传统的菜单仓库
sudo add-apt-repository ppa:diesch/testing

#添加docky的仓库
sudo add-apt-repository ppa:noobslab/icons

#添加主题flatabulous-theme的仓库
sudo add-apt-repository ppa:noobslab/themes


#更新仓库
sudo apt update
sudo apt install docky && sudo apt install flatabulous-theme && sudo apt install ultra-flat-icons 















