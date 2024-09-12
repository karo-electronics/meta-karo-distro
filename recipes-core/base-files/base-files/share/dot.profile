# ~/.profile: executed by Bourne-compatible login shells.

if [ -f ~/.bashrc ]; then
  . ~/.bashrc
fi

# path set by /etc/profile
# export PATH

if [ -n "$XDG_RUNTIME_DIR" -a -d "$XDG_RUNTIME_DIR" ];then
  export WAYLAND_DISPLAY="$(cd "$XDG_RUNTIME_DIR";shopt -s nullglob;ls wayland-?)"
fi

# Might fail after "su - myuser" when /dev/tty* is not writable by "myuser".
mesg n 2>/dev/null
