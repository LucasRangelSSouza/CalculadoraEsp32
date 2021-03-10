#include <Wire.h>                 //Bibioteca para lidar com a comunicação I2C utilizada pelo display Oled
#include <Adafruit_GFX.h>         //TIRARRR
#include <Adafruit_SSD1306.h>     //Biblioteca da Adafuit com funçoes para trabalhar com o display Oled

/* Aqui instanciamos a Adafruit_SSD1306, passando para o metodo construtor da mesma
 * As dimensoes em pixels do display (128x64), e o ponteiro da biblioteca Wire, 
 * o ultimo argumento é o pino de reset do display, Nosso exemplar não contem um, por isso passamos -1
 * Indicando que não ha um pino de reset
 * 
 * Não foi necessario definir os pinos 22(SDA) e 21(SCL) do protocolo I2C, pois o nucleo do ESPIDF instalado no
 * backend do arduino já define a pinagem padrão do I2C, a biblioteca wire por sua vez, já utiliza internamente
 * estes defines.
 */
Adafruit_SSD1306 display(128, 64, &Wire, -1);
/*
 *Vetor contendo os caracteres da Matriz do teclado
 */
String digitos[5][4]={
{ "1", "2", "3", "/" },/*LinhaA*/
{ "4", "5", "6", "*" },/*LinhaB*/
{ "7", "8", "9", "-" },/*LinhaC*/
{ "C", "0", "=", "+" },/*LinhaD*/
};

/*
 * Pinagem das linhas e colunas do teclado matricial
 */
const int colunas[4]={5, 4, 2, 15};
const int linhas[4]= {27,14,12,13};


String ConteudoDisplay = "";  //Variavel que contem todo o texto digitado no display

/*Todos os calculos são realizados passo a passo de 2 em duas operaçoes
 * Abaixo temos as 3 variaveis de uma operaçao, o primeiro numero
 * o segundo numero e o operador matematico
 */
String PrimeiroNumero = "0";  
String SegundoNumero = "0";
String UltimoOperador = "";

/*Variavel que auxilia no controle de operaçoes,
 * Quando o botão de um operador é pressionado, entende-se que o usuario digitou
 * o primeiro numero da operação e agora ira digitar o proximo, essa variavel
 * boleana indica se o primeiro numero já foi digitado ou não
 */
bool PrimeiroNumeroFinalizado = 0;


void ConfiguraTeclado(){
  /*Este ciclo for intera todos pinos do teclado 
   Colocando todos os pinos das linhas como saida (output) e os pinos das colunas como entrada (input)
   E logo apos setar como saida as linhas, seta nivel logico baixo nas linhas*/
   
   for(int i=0;i<=3;i++){
     pinMode(linhas[i], OUTPUT); // seta todos as linhas do teclado como saída 
     pinMode(colunas[i], INPUT); // seta todos as colunas do teclado como entrada
     digitalWrite(linhas[i], LOW);
  }
 }


void ConfiguraDisplay(){
  /* Aqui o display é inializado
   *O metodo begin do objeto display (o qual é uma instancia de Adafruit_SSD1306) recebe 2 argumentos
   *O primeiro é a voltagem que o display ira trabalhar SSD1306_SWITCHCAPVCC é um pseudonimo de 3.3
   *definido pela propria Adafruit_SSD1306, ou seja o display irá trabalhar em 3.3v
   *O segundo argumento é o endereço I2C o qual o display está conectado, no nosso caso é 0x3C
   *A biblioteca wire, o qual foi passada anteriormente para o objeto display, irá procurar esse endereço
   *e ao obter um retorno do display ira retornar 0 (true)
   */
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { 
    Serial.println(F("Display SSD1306 não Conectado"));
    for(;;);
  }

  /*Limpa qualquer dado no display oled, um ciclo varre todo array de pixels e os define em 
   * nivel logico baixo, desligando os pixels
   */
  display.clearDisplay();
  
  SplashScreen();

  //Define o tamanho da fonte de texto utilizada para escrever no display
  display.setTextSize(1);

  /*O display pode trabalhar com Black e White, 
   * Ao definirmos white, se liga os pixels dos caracteres escritos, se
   * definirmos como Black, a tela "limpa" contem todos os pixels ligados
   * e os caracteres são escritos desligando-se os pixels
   */
  display.setTextColor(WHITE);
  
 }


void SplashScreen(){
  /*
   * Função que desenha o logo do IFG ao iniciar ou limpar a tela
   **/
  
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  
  //Desenha um circulo, nas coodenadas cartesianas x=48 e y=6, e com raio 6
  display.fillCircle(48, 6, 6, WHITE);

  /*Desenha os quadrados com cantos arredondados iniciando das
   * Coordenadas passadas nos dois primeiros argumentos, o terceiro argumento é o
   * tamanho da base do quadrado e o quarto o tamanho da altura, o quinto argumento 
   * é o raio da borda do quadrado
   */
  display.fillRoundRect(42,15,  12,12,  2, WHITE);
  display.fillRoundRect(42,29,  12,12,  2, WHITE);
  display.fillRoundRect(42,43,  12,12,  2, WHITE);

  display.fillRoundRect(57,1,  12,12,  2, WHITE);
  display.fillRoundRect(57,15,  12,12,  2, WHITE);
  display.fillRoundRect(57,29,  12,12,  2, WHITE);
  display.fillRoundRect(57,43,  12,12,  2, WHITE);

  display.fillRoundRect(72,1,  12,12,  2, WHITE);
  display.fillRoundRect(72,15,  12,12,  2, WHITE);

  //Posiciona o cursor na posição x=30 e y=57
  display.setCursor(30, 57);
  //Escreve o texto na clculadora iniciando da posição do cursor
  display.println("CALCULADORA");

  //Aplica todas as alteraçoes no display
  display.display();
  
  }

//Le a tecla digitada no teclado
String LerTeclaDigitada(){
  /*
   * Captura e identifica a tecla digitada no teclado matricial
   */
 for (int linha = 0; linha <= 3; linha++) {
    digitalWrite(linhas[linha], HIGH);
    for (int coluna = 0; coluna <= 3; coluna++) {
      if(digitalRead(colunas[coluna])){
        return digitos[coluna][linha];
       }
      }
    digitalWrite(linhas[linha], LOW);
    if (linha == 3) {
      linha = -1;
    }
  } 
}

void ApresentarNoDisplay(String conteudo, int fonte){
  /*
   * Imprime do display o conteudo passado como argumento
   */
  display.clearDisplay();
  display.setTextSize(fonte);
  display.setCursor(0, 10);
  display.println(conteudo);
  display.display();
  }

String RealizaOperacao(String primeiroNumero,String segundoNumero,String operador){
  /*
   * Recebe 3 Strings, 2 contendo os numeros a serem operados e outra string como o operador matematico
   */
  if(operador == "+"){
      return String((primeiroNumero.toFloat()+segundoNumero.toFloat()),1);
    }else if(operador == "-"){
      return String((primeiroNumero.toFloat()-segundoNumero.toFloat()),1);
    }else if(operador == "/"){
      return String((primeiroNumero.toFloat()/segundoNumero.toFloat()),1);
    }else if(operador == "*"){
      return String((primeiroNumero.toFloat()*segundoNumero.toFloat()),1);
    }
  }
void ProcessaTeclaDigitada(String teclaDigitada){
  
  if((teclaDigitada == "-") || (teclaDigitada == "/") || (teclaDigitada == "*") ||(teclaDigitada == "+")){
    ConteudoDisplay+=teclaDigitada;
    if(PrimeiroNumeroFinalizado){
      PrimeiroNumero=RealizaOperacao(PrimeiroNumero,SegundoNumero,UltimoOperador);
      SegundoNumero="0";
      UltimoOperador = teclaDigitada;
      
     }else{
       PrimeiroNumeroFinalizado = true;
       UltimoOperador = teclaDigitada;  
     }
    ApresentarNoDisplay(ConteudoDisplay,3);
  }else if(teclaDigitada == "=")
  {
    ConteudoDisplay = RealizaOperacao(PrimeiroNumero,SegundoNumero,UltimoOperador);
    PrimeiroNumero = ConteudoDisplay;
    SegundoNumero = "0.0";
    UltimoOperador = "+";
    PrimeiroNumeroFinalizado = true;
    ApresentarNoDisplay(ConteudoDisplay,4);
    
  }else if(teclaDigitada == "C")
  {
    ConteudoDisplay = "";
    PrimeiroNumero = "0";
    SegundoNumero = "0";
    UltimoOperador = "";
    PrimeiroNumeroFinalizado = false;
    SplashScreen();
    
  }else{
    //É um numero
    ConteudoDisplay+=teclaDigitada;
    ApresentarNoDisplay(ConteudoDisplay,3);   
    if(PrimeiroNumeroFinalizado){
      //O primeiro numero da conta já foi escrito e um operador digitado
      SegundoNumero+=teclaDigitada;
      }else{
        //Primeiro numero ainda não foi finalizado
        PrimeiroNumero+=teclaDigitada;    
      }  
     
  }
  
  }
void setup() {
  Serial.begin(115200);
  ConfiguraDisplay();
  ConfiguraTeclado();
}

void loop() {
  String teclaDigitada = LerTeclaDigitada();
  ProcessaTeclaDigitada(teclaDigitada);
  delay(500);
}
