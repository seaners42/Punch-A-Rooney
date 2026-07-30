#!/bin/sh

grit Arena2.png Arena3.png Arena4.png Arena5.png Arena6.png Audience.png -ftb -fh! -gTFF00FF -gt -gB8 -mRa -mLa -pS -OAudience


for file in *.bin; do
  mv -- "$file" "${file%.bin}"
  echo "Moving $file"
done

cp Audience.pal Arena2.pal
cp Audience.pal Arena3.pal
cp Audience.pal Arena4.pal
cp Audience.pal Arena5.pal
cp Audience.pal Arena6.pal

mv *.pal *.img *.map ../../nitrofiles/bg
