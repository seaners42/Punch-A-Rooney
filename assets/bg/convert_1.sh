#!/bin/sh

grit Arena1.png Gym.png -ftb -fh! -gTFF00FF -gt -gB8 -mRa -mLa -pS -OGym

for file in *.bin; do
  mv -- "$file" "${file%.bin}"
  echo "Moving $file"
done

cp Gym.pal Arena1.pal

mv *.pal *.img *.map ../../nitrofiles/bg
