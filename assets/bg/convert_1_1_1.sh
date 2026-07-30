#!/bin/sh

grit Title.png MenuBg.png ChampMenuBg.png -ftb -fh! -gTFF00FF -gt -gB8 -mRa -mLa -pS -OTitle

grit Preview1.png Preview1Champ.png PreviewBg1.png -ftb -fh! -gTFF00FF -gt -gB8 -mRa -mLa -pS -OPreview1
grit Preview2.png Preview2Champ.png PreviewBg2.png -ftb -fh! -gTFF00FF -gt -gB8 -mRa -mLa -pS -OPreview2
grit Preview3.png Preview3Champ.png PreviewBg3.png -ftb -fh! -gTFF00FF -gt -gB8 -mRa -mLa -pS -OPreview3
grit Preview4.png Preview4Champ.png PreviewBg4.png -ftb -fh! -gTFF00FF -gt -gB8 -mRa -mLa -pS -OPreview4
grit Preview5.png Preview5Champ.png PreviewBg5.png -ftb -fh! -gTFF00FF -gt -gB8 -mRa -mLa -pS -OPreview5
grit AWinnerIsYou.png AChampIsYou.png PreviewBg0.png -ftb -fh! -gTFF00FF -gt -gB8 -mRa -mLa -pS -OAWinnerIsYou

grit Timeout1.png Timeout2.png Timeout3.png Timeout4.png Timeout5.png Timeout6.png TimeoutRooney.png TimeoutSlimJim.png TimeoutJill.png TimeoutPablo.png TimeoutSvetlana.png TimeoutKairo.png TimeoutRooneyChamp.png TimeoutSlimJimChamp.png TimeoutJillChamp.png TimeoutPabloChamp.png TimeoutSvetlanaChamp.png TimeoutKairoChamp.png -ftb -fh! -gTFF00FF -gt -gB8 -mRa -mLa -pS -OTimeout1
grit Partial0.png Partial1.png Partial2.png Partial3.png Partial4.png Partial5.png -ftb -fh! -gTFF00FF -gt -gB8 -mRa -mLa -pS -OPartial0

for file in *.bin; do
  mv -- "$file" "${file%.bin}"
  echo "Moving $file"
done

cp Preview1.pal PreviewBg1.pal
cp Preview2.pal PreviewBg2.pal
cp Preview3.pal PreviewBg3.pal
cp Preview4.pal PreviewBg4.pal
cp Preview5.pal PreviewBg5.pal
cp AWinnerIsYou.pal PreviewBg0.pal

cp Preview1.pal Preview1Champ.pal
cp Preview2.pal Preview2Champ.pal
cp Preview3.pal Preview3Champ.pal
cp Preview4.pal Preview4Champ.pal
cp Preview5.pal Preview5Champ.pal
cp AWinnerIsYou.pal AChampIsYou.pal

cp Partial0.pal Partial1.pal
cp Partial0.pal Partial2.pal
cp Partial0.pal Partial3.pal
cp Partial0.pal Partial4.pal
cp Partial0.pal Partial5.pal

cp Timeout1.pal Timeout2.pal
cp Timeout1.pal Timeout3.pal
cp Timeout1.pal Timeout4.pal
cp Timeout1.pal Timeout5.pal
cp Timeout1.pal Timeout6.pal

cp Timeout1.pal TimeoutRooney.pal
cp Timeout1.pal TimeoutSlimJim.pal
cp Timeout1.pal TimeoutJill.pal
cp Timeout1.pal TimeoutPablo.pal
cp Timeout1.pal TimeoutSvetlana.pal
cp Timeout1.pal TimeoutKairo.pal

cp Timeout1.pal TimeoutRooneyChamp.pal
cp Timeout1.pal TimeoutSlimJimChamp.pal
cp Timeout1.pal TimeoutJillChamp.pal
cp Timeout1.pal TimeoutPabloChamp.pal
cp Timeout1.pal TimeoutSvetlanaChamp.pal
cp Timeout1.pal TimeoutKairoChamp.pal

cp Title.pal MenuBg.pal
cp Title.pal ChampMenuBg.pal

mv *.pal *.img *.map ../../nitrofiles/bg
