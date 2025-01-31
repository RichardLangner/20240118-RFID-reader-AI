Uses a tag-ID reader module connected to a Wemos D1 mini, to read the tags found on animals.
When a tag is sensed, the module sends the tag-ID to the Wemos over software serial.
The Wemos cleans the number after removing the start byte (2), the end byte(3) and any unprintable characters (/r and /n).
The result is a 14 digit number.
LittleFS stores files for each tag-ID, using the tag-ID as the filename. In the file is the animal number (1,2,3,..).
The on board LED blinks out the number.
If no number is sensed the blinked digit is zero (1s on 1s off).
The code is non-blocking.
