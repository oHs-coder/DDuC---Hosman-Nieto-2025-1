#define BOTON 2
const float R1 = 100000.0;  // Ohmios
const float R2 = 6800.0;   // Ohmios
const int EntradaV = A0;    // Pin de entrada analógica
const int SALIDAS[] = {3, 4, 5, 6, 7};
const int NUM_SALIDAS = sizeof(SALIDAS) / sizeof(SALIDAS[0]);
const int NUM_COMBINACIONES = 9;

const byte COMBINACIONES[NUM_COMBINACIONES][NUM_SALIDAS] = {
    {1, 0, 0, 0, 0},  // Estado Shunt
    {0, 1, 0, 0, 0},  // Estado Pot
    {0, 0, 1, 0, 0},  // Estado R1
    {0, 0, 0, 1, 0},  // Estado R2
    {0, 0, 0, 0, 1},  // Estado R3
    {0, 0, 1, 1, 1},  // Estado R1+R2+R3
    {0, 0, 1, 1, 0},  // Estado R1+R2
    {0, 0, 1, 0, 1},  // Estado R1+R3
    {0, 0, 0, 1, 1},  // Estado R2+R3
};

int estadoActual = 0;
bool estadoBotonAnterior = HIGH;
unsigned long ultimaPulsacion = 0;
const unsigned long debounceDelay = 50;

void setup() {
    Serial.begin(9600);
    analogReference(INTERNAL);
    pinMode(EntradaV, INPUT);
    pinMode(BOTON, INPUT_PULLUP);
    for (int i = 0; i < NUM_SALIDAS; i++) {
        pinMode(SALIDAS[i], OUTPUT);
    }
    actualizarSalidas();
}

void loop() {
    gestionarBoton();
    medirVoltaje();
    delay(500);
}

void gestionarBoton() {
    bool estadoBoton = digitalRead(BOTON);
    if (estadoBoton == LOW && estadoBotonAnterior == HIGH && millis() - ultimaPulsacion > debounceDelay) {
        ultimaPulsacion = millis();
        estadoActual = (estadoActual + 1) % NUM_COMBINACIONES;
        actualizarSalidas();
    }
    estadoBotonAnterior = estadoBoton;
}

void actualizarSalidas() {
    for (int i = 0; i < NUM_SALIDAS; i++) {
        digitalWrite(SALIDAS[i], COMBINACIONES[estadoActual][i]);
    }
}

void medirVoltaje() {
    double VinProm = 0;
    for (int i = 0; i < 150; i++) {
        double Vout = (analogRead(EntradaV) * 1.1) / 1023;
        VinProm += Vout*((R1 + R2)/R2);
    }
    Serial.print("Voltaje: ");
    Serial.print(VinProm / 150, 3);
    Serial.println(" V");
} 