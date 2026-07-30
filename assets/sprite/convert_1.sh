#!/bin/sh

grit bars.png boxerbars.png portraits.png champportraits.png taphelp.png -ftb -fh! -gTFF00FF -gt -gB8 -m! -pS -Obottomfight

for file in *.bin; do
  mv -- "$file" "${file%.bin}"
  echo "Moving $file"
done

mv *.pal *.img ../../nitrofiles/sprite
