/** 
 *   Nome: Pedro Leonel de Lorena, Leonardo Ferrarese Correa, Lais Rodrigues Sevilhano & Luigi Arnosti Reginato
 *  Descrição: Neste modulo de publish nos coletamos os hex do controle remoto e fizemos com que toda vez que apertasse o botao ele emitsse para a tela oq o hex faz.
 *  Projeto: Modulo de publicar os comandos feito na tela retratil
 *  Data: 21/05
 *  Versão: 0.0.6
 */

#include <Arduino.h>
#include "WiFiManager.h"
#include "MqttManager.h"
#include "DebugManager.h"
#include <ArduinoJson.h>
#include <secrets.h>
#include <Bounce2.h>
//=========================
//* PINOS

const int pinUp = 2;
const int pinDown = 3;
const int pinPause = 4;

//=========================

//=========================
//* Variaveis globais

unsigned long frequenciaUp = 433;
unsigned long frequenciaDown = 432;
unsigned long frequenciaPause = 431;

const unsigned long tempoInicio = 0;
unsigned long tempo = 0;
unsigned long tempoDecorrido = 0;
unsigned long tempoPause;
unsigned long tempoMaximo = 20000;
bool tempoParado = false;

const char topicoComandoUp[] = "senai134/publisherTelaRetratil/esp32/comando";
//=========================

//=========================
//* CRIAÇÃO DE OBJETOS

Bounce UP = Bounce();
Bounce DOWN = Bounce();
Bounce PAUSE = Bounce();
//=========================

//=========================
//* INVOCAR AS FUNÇÕES

void botaoUp();
void botaoDown();
void botaoPause();
void postarComando();

//=========================

void setup()
{
  //=========================
  //* pinMODE
 // UP.attach(pinUp, INPUT_PULLUP);
 // DOWN.attach(pinDown, INPUT_PULLUP);
  //PAUSE.attach(pinPause, INPUT_PULLUP);
  pinMode(pinUp, INPUT_PULLUP);
  pinMode(pinDown, INPUT_PULLUP);
  pinMode(pinPause, INPUT_PULLUP);
  //=========================

  configurarDebug();
  conectarWiFi();
  configurarMQTT();
  conectarMQTT();
}

void loop()
{
  garantirWiFiConectado();
  garantirMQTTConectado();
  loopMQTT();

  UP.update();
  DOWN.update();
  PAUSE.update();

  //TODO: TROCAR OS IFS DO BOTAO PARA OQ VAI VIM DO OUTRO JSON POIS ELE VAI FALAR SE FOI PRESSIONADO OU NAO.
  if (UP.fell())
  {
    botaoUp();
  }
  if (DOWN.fell())
  {
    botaoDown();
  }
  if (PAUSE.fell())
  {
    botaoPause();

  }
  
}


/*
  A ideia é que o botaoUP e down tenha a mesma logica pois ambos tem a mesma função, só que um sobe e o outro desce, e o botaoPAUSE tem a função de pausar o tempo, ou seja, quando ele for apertado ele salva o tempo decorrido e para de contar, e quando ele for apertado novamente ele continua contando de onde parou.
*/
void botaoUp()
{
  if (frequenciaUp != 433)
    debugErro("A frequência do botão UP está errada, favor checar se está correta");

  if (frequenciaUp == 433)
  {
    if (tempoParado)
    {
      debugInfo("Tela retrátil subindo");
      tempo = millis() - tempoPause;
      tempoParado = false;
      // Serial para debug
    debugInfo("Tempo decorrido: " + String(tempoDecorrido) + " ms");
    }
    tempoDecorrido = millis() - tempo;

    if (tempoDecorrido >= tempoMaximo)
    {
      debugInfo("Tela retrátil subiu por completo.");
      tempoDecorrido = 0;
      tempoParado = false;
    }
  }
}

void botaoPause()
{
  if (frequenciaPause != 431)
    debugErro("A frequência do botão Pause está errada, favor checar se está correta");

  if (frequenciaPause == 431)
  {
    debugInfo("Tela retrátil pausada");
    tempoParado = true;
    tempoPause = tempoDecorrido;
  }
}

void botaoDown()
{
  if (frequenciaDown != 432)
    debugErro("A frequência do botão DOWN está errada, favor checar se está correta");

  if (frequenciaDown == 432)
  {
    if (tempoParado)
    {
      debugInfo("Tela retrátil descendo");
      tempo = millis() - tempoPause;
      tempoParado = false;

       // Serial para debug
    debugInfo("PAUSADO em: " + String(tempoPause) + " ms");
    }
    tempoDecorrido = millis() - tempo;

    if (tempoDecorrido >= tempoMaximo)
    {
      debugInfo("Tela retrátil desceu por completo.");
      tempoDecorrido = 0;
      tempoParado = false;
    }
  }
}