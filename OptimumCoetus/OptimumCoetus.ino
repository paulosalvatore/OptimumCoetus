/*
Name:		OptimumCoetus.ino
Created:	9/5/2017 2:43:53 PM
Author: 	Paulo Salvatore
*/

#include <Servo.h>
#include <Mindwave.h>
#include <SimpleTimer.h>

SimpleTimer simpleTimer;
Mindwave mindwave;

Servo servoX;
int portaServoX = 9;

Servo servoY;
int portaServoY = 10;

int botaoLiberarMovimento = 4;
bool liberarMovimentoAutomatico = false; // Caso queira liberar o movimento automaticamente logo após ele acabar
bool botaoLiberarMovimentoLiberado = true;

unsigned long tempoAtual = 0;

bool movimentoIniciado = false;

int atencaoAtivarMovimento = 60;

// Variáveis do Motor X
int posicaoMotorX;
int posicaoMotorXInicial = 0; // Em qual posição o motor X irá começar
int intervaloMovimentoMotorX = 10; // Quantos graus cada passo vai andar
int posicaoFinalMotorX = 180; // Qual o ângulo final do motor X

// Variáveis do Motor Y
int posicaoMotorY;
int posicaoMotorYInicial = 0; // Em qual posição o motor Y irá começar
int intervaloMovimentoMotorY = 10; // Quantos graus cada passo vai andar
int posicaoFinalMotorY = 180; // Qual o ângulo final do motor Y

/*
* intervaloMovimentacaoMotor = A cada quantos milissegundos você quer
* que ele faça uma movimentação no motor.
*/
int intervaloMovimentacaoMotor = 500;

/*
* delayAguardando = Quantos milissegundos você quer que ele fique
* esperando após terminar a movimentação
*/
long delayAguardando = 5000;
long delayMovimentacao = intervaloMovimentacaoMotor * (posicaoFinalMotorX / intervaloMovimentoMotorX);

void setup()
{
	posicaoMotorX = posicaoMotorXInicial;
	posicaoMotorY = posicaoMotorYInicial;

	servoX.attach(portaServoX);
	servoX.write(posicaoMotorX);

	servoY.attach(portaServoY);
	servoY.write(posicaoMotorY);

	mindwave.setup();
	mindwave.setDebug(true);

	Serial.begin(9600);
}

void IniciarMovimento()
{
	if (movimentoIniciado)
		return;

	Serial.println("Iniciar Movimento");

	movimentoIniciado = true;

	simpleTimer.setTimeout(intervaloMovimentacaoMotor, MoverMotorX);

	simpleTimer.setTimeout(delayMovimentacao + intervaloMovimentacaoMotor, MoverMotorY);

	simpleTimer.setTimeout(delayAguardando + (delayMovimentacao * 2), ReiniciarMotores);
}

void EncerrarMovimento()
{
	movimentoIniciado = false;
}

void MoverMotorX()
{
	Serial.println("MoverMotorX");

	posicaoMotorX += intervaloMovimentoMotorX;

	servoX.write(posicaoMotorX);

	if (posicaoMotorX < posicaoFinalMotorX)
		simpleTimer.setTimeout(intervaloMovimentacaoMotor, MoverMotorX);
}

void MoverMotorY()
{
	Serial.println("MoverMotorY");

	posicaoMotorY += intervaloMovimentoMotorY;

	servoY.write(posicaoMotorY);

	if (posicaoMotorY < posicaoFinalMotorY)
		simpleTimer.setTimeout(intervaloMovimentacaoMotor, MoverMotorY);
}

void ReiniciarMotores()
{
	Serial.println("ReiniciarMotores");

	posicaoMotorX = posicaoMotorXInicial;
	servoX.write(posicaoMotorX);

	posicaoMotorY = posicaoMotorYInicial;
	servoY.write(posicaoMotorY);

	if (liberarMovimentoAutomatico)
		simpleTimer.setTimeout(intervaloMovimentacaoMotor, EncerrarMovimento);
}

void ChecarBotaoLiberarMovimento()
{
	if (!movimentoIniciado)
		return;

	bool leituraBotaoLiberarMovimento = digitalRead(botaoLiberarMovimento);
	if (botaoLiberarMovimentoLiberado && leituraBotaoLiberarMovimento)
	{
		EncerrarMovimento();

		botaoLiberarMovimentoLiberado = false;
	}
	else if (!botaoLiberarMovimentoLiberado && !leituraBotaoLiberarMovimento)
		botaoLiberarMovimentoLiberado = true;
}

void loop()
{
	simpleTimer.run();

	tempoAtual = millis();

	if (Serial.available() > 0)
	{
		char valorRecebido = Serial.read();

		Serial.print("Valor Recebido: ");
		Serial.println(valorRecebido);

		if (valorRecebido == 'a')
			IniciarMovimento();
	}

	ChecarBotaoLiberarMovimento();

	mindwave.update();

	if (mindwave.getQuality() == 200)
	{
		if (mindwave.getAttention() >= atencaoAtivarMovimento)
		{
			IniciarMovimento();
		}
	}
}
