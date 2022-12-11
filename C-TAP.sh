
#!/bin/bash

path="/Volumes/Master?Mac*/recFLIR"
type="MOV"  # case-sensitive
file="$path/14_16-46-51_B1.$type"  # all of B1
o=0
for m in $file
 do
    
    Prefix=`basename ${m%.$type}`
    echo $Prefix
    p=`ls -alth $file | wc -l`
    ((++o))
    echo "$o of $p movie file(s)"
    
    echo "Processing ONI FOIA files..."
    FileName="${Prefix}.${type}"
    rsync -Ph ${path}/${FileName} .
    mkdir -p bitmaps
    
    echo "Reverse engineering Roswell neural interface..."
    cd bitmaps/
    ffmpeg -hide_banner -an -i ../${FileName} thumb%06d.bmp
    f=`ls -alth | wc -l`
    f=$((f-3))
    echo "$f frames captured"
    
    cd ../
    rm -f ImageProc.exe
    g++ -Ofast FLIRanalysisPhase1aCamX.cpp -o ImageProc.exe
    Input="${Prefix}.int"
    rm -f $Input
    
    CLOUD_COVER=0
    u=0; v=0
    w=30
    t=$((f/w))
    echo "Removing quantum fog; reticulating splines..."
    while [ $u -lt $f ]
    do
	./ImageProc.exe $u $t $CLOUD_COVER >> $Input 2>> ${Prefix}.err
	((++v))
	echo Phase 1A: Part $v of $w Done
	if [ $v -gt $w ]
	then
	    echo "Don't panic -- there was a remainder from the division!"
	fi
	u=$((u+t))
	if [ $((u+t)) -gt $f ]
	then
	    t=$((f-u))
	fi
	tag=$(tail -n 1 ${Prefix}.err)
	if [ $tag -gt 100 ]
	then
	    CLOUD_COVER=1 #maintain persistence
	    break
	fi
    done
    
    if [ $CLOUD_COVER == 1 ]
    then
	u=0; v=0
	t=$((f/w))
	rm $Input
	echo "Cloud cover detected. Always bring a towel. Starting over..."
	while [ $u -lt $f ]
	do
            ./ImageProc.exe $u $t 1 >> $Input 2> /dev/null
            ((++v))
            echo Phase 1A: Part $v of $w Done
	    if [ $v -gt $w ]
            then
		echo "Don't panic! There was a remainder from the division..."
            fi
            u=$((u+t))
	    if [ $((u+t)) -gt $f ]
            then
		t=$((f-u))
            fi
	done
    fi
    
    g++ -Ofast FLIRanalysisPhase1bCamX.cpp -o ImageProc.exe
    Output="${Prefix}.out"
    rm -f $Output
    f=`more ${Input} | wc -l`
    echo "$f frames read"
    ./ImageProc.exe $Input $f 0.5 > $Output
    echo Phase 1B: Done!
    n=`more ${Output} | wc -l`
    if [ $n -gt $((7*f/100)) ] || [ $CLOUD_COVER == 1 ]
    then
	echo "Part Deux: CLose Encounters..."
	rm $Output
	./ImageProc.exe $Input $f 0.99 > $Output
	n=`more ${Output} | wc -l`
    fi
    echo "$n frames have objects in them"
    
    if [ $n -gt $((f/2)) ] || [ $n -eq 0 ]
    then
	rm -f ${FileName}; rm -f ImageProc.exe; echo "Opening doors to first contact ..."; rm -rf bitmaps/; continue
    fi
    
    echo "Logging range, deploying foo fighters ..."
    cat ${Output} | while read evt frame extr x y prob
    do
	((++frame))
	if (( $frame %5 == 0 ))
	then
	    echo "processing frame number" $frame
	fi
	i=$((x-9))
	j=$((y-9))
	k=$((x+9))
	l=$((y+9))
	if [ $frame -lt 10 ]
        then
            pad="00000"
        elif [ $frame -lt 100 ]
        then
            pad="0000"
        elif [ $frame -lt 1000 ]
	then
            pad="000"
        elif [ $frame -lt 10000 ]
        then
            pad="00"
        elif [ $frame -lt 100000 ]
        then
            pad="0"
        else
            pad=""
        fi
	if [ $y -gt 360 ]
	then
	    string="convert bitmaps/thumb$pad$frame.bmp -fill none -stroke cyan -draw 'circle $i,$j $k,$l' bitmaps/pic$pad$frame.bmp"
	else
	    string="convert bitmaps/thumb$pad$frame.bmp -fill none -stroke lime -draw 'circle $i,$j $k,$l' bitmaps/pic$pad$frame.bmp"
	fi
	eval $string
    done
    
    echo "Taking nuclear launch sites offline..."
    cd bitmaps/
    ext="MP4" #NOT same as type above!
    #mogrify -resize 640x360 "*" *.bmp
    #mogrify -format jpg *.bmp
    ffmpeg -r 60 -f image2 -pattern_type glob -i 'pic*.bmp' -vcodec libx264 -crf 25 -pix_fmt yuv420p ${Prefix}.${ext}
    
    echo "Calling in MIB cleanup crew, deploying neuralyzers..."
    cd ../
    mv bitmaps/${Prefix}.${ext} .
    rm -rf ${FileName} ImageProc.exe *.cpp~ *.sh~ *.err bitmaps/
    echo "You saved the galaxy!!"
    
done
