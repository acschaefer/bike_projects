// Read VCC input voltage [mV] using the interal precision voltage reference of 1.1 V.
long readVcc() 
{
    // Read the 1.1 V reference against VCC. 
    ADMUX = _BV(REFS0) | _BV(MUX3) | _BV(MUX2) | _BV(MUX1); 
    
    // Wait for reference voltage to settle.
    delay(2);
    ADCSRA |= _BV(ADSC); 
    
    // Convert.
    while (bit_is_set(ADCSRA, ADSC)); 
    
    // Back-calculate VCC in [mV].
    long result = ADCL;
    result |= ADCH<<8; 
    result = 1126400l / result;
    return result; 
}

void setup() 
{
    Serial.begin(9600); 
}

void loop() 
{ 
    Serial.println( readVcc(), DEC ); 
    delay(1000); 
}
