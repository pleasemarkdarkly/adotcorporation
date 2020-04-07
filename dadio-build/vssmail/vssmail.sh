#!/bin/bash
#
# vssmail.sh: send out an email every time a vss action occurs
# Dan Conti danc@iobjects.com 02/08/01
#
# This is an initial, 'dumb' version of the script which just
# knows that a change occurred and will mail based on that change.
# In the future, it would be nice for this script to add some interpretation
# to that change, and group or filter out changes as needed.
#

# adjust these variables for a different VSS database
VSS_LOGFILE="/c/vssmail/vss_logs/io_dadio.log"
VSS_COUNTFILE="/c/vssmail/logfile_data/io_dadio.count"

SLEEP_DURATION=30

# you shouldn't need to modify these
BLAT_MAILER="./blat.exe"
OUTPUT_FILE="./tmp.txt"
BLAT_ARGS="-to vssmail@iobjects.com"
BYTE_COUNT="wc --bytes"

CNT=0
NEWCNT=0
DIFF=0
SUBJECT=

if [ ! -f $VSS_COUNTFILE ]; then
  echo 0 > $VSS_COUNTFILE
fi

while true; do
  CNT=`cat $VSS_COUNTFILE`
  NEWC=`$BYTE_COUNT $VSS_LOGFILE`
  NEWCNT=`echo $NEWC | cut -f 1 -d ' '`

  # tail is safe for $NEWCNT==$CNT - do this here to reduce timing issues
  tail -c $(($NEWCNT-$CNT)) $VSS_LOGFILE > $OUTPUT_FILE

  # have new bytes appeared?
  if [ $CNT -lt $NEWCNT ]; then
    # yes, so mail
    SUBJECT=`head -n 1 $OUTPUT_FILE`
    SUBJECT="vss checkin: $SUBJECT"
    SENDER=`head -n 3 $OUTPUT_FILE | grep User | cut -f 2 -d " "`

    if [ "a$SENDER" == "a" ]; then
      SENDER="VSS Mailer"
    fi
#    echo -e "\r\n.\r\n" >> $OUTPUT_FILE

    $BLAT_MAILER $OUTPUT_FILE $BLAT_ARGS -subject "$SUBJECT" -from "$SENDER"
    echo $NEWCNT > $VSS_COUNTFILE
  fi

  sleep $SLEEP_DURATION
done


