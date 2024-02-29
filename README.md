# Colorless Memory
A multiplayer memory with design from Colorless Odyssey

## Error fix

### libcuda.so.1 is not a symbolic link on WSL2

Open powershell as admin and run `cmd` or cmd in admin mod

```
cd \Windows\System32\lxss\lib
del libcuda.so
del libcuda.so.1
```

Work on Cmder but not powershell
```
mklink libcuda.so libcuda.so.1.1
mklink libcuda.so.1 libcuda.so.1.1
```

Rerun the command that give this error to finish it