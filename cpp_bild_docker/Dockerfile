from archlinux/base
run  echo -e "[archlinuxcn]\nServer = https://mirrors.tuna.tsinghua.edu.cn/archlinuxcn/\$arch" >> /etc/pacman.conf && \
     echo Server = https://mirrors.tuna.tsinghua.edu.cn/archlinux/\$repo/os/\$arch > /etc/pacman.d/mirrorlist && \
     pacman -Syy && \
     pacman -S archlinuxcn-keyring --noconfirm && \
     pacman -S cmake git gcc gdb fish make  --noconfirm
cmd ["fish"]
