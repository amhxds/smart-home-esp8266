 char read_ssid_length (void) {
    return EEPROM.read(64);
}

 char read_pass_length (void) {
    return EEPROM.read(65);
}

void read_ssid ( char* ssid) {
     char length = read_ssid_length();
     char counter = 0;
    for (counter = 0; counter<32; counter++) {
        if (counter<length) 
            ssid[counter] = EEPROM.read(counter);
        else
            ssid[counter] = ' ';
    }
}

void read_pass ( char* pass) {
     char length = read_pass_length();
     char counter = 0;
    for (counter = 32; counter<64; counter++) {
        if (counter-32<length) 
            pass[counter-32] = EEPROM.read(counter);
        else
            pass[counter-32] = ' ';
    }
}

void write_ssid ( char* ssid) {
     char length = 0;
     char counter = 0;
    for (counter = 0; counter<32; counter++) {
        EEPROM.write(counter, ssid[counter]);
		delay(100);
        if (ssid[counter] != ' ')
            length++;
    }
    EEPROM.write(64, length);
	delay(100);
    EEPROM.commit();

}

void write_pass ( char* pass) {
     char length = 0;
     char counter = 0;
    for (counter = 32; counter<64; counter++) {
        EEPROM.write(counter, pass[counter-32]);
		delay(100);
        if (pass[counter-32] != ' ')
            length++;
    }
    EEPROM.write(65, length);
	delay(100);
    EEPROM.commit();
}



char read_char (int address) {
  return EEPROM.read(address+65);
}




void write_char (int source, char value) {
  EEPROM.write(source+65, value);
  delay(20);
  EEPROM.commit();
}
