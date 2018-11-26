# OBData
Um software em Qt para aquisição e plotagem de dados de um adaptador OBD-II<br />
A Qt application to read and plot data from an OBD-II adapter


###### ESTE REPOSITÓRIO FAZ PARTE DA DISCIPLINA DE TÓPICOS EM PROJETO AUTOMOTIVO, COM ÊNFASE EM REDES VEICULARES, DA UNIVERSIDADE FEDERAL DE SANTA CATARINA (UFSC)
###### THIS REPOSITORY IS PART OF A SCHOOL PROJECT UNDER THE SUBJECT OF VEHICULAR NETWORKING, BY THE FEDERAL UNIVERSITY OF SANTA CATARINA (UFSC)

*Recomenda-se a leitura do relatório.

### Pré-Requisitos e Configuração Inicial
- Computador com sistema operacional Linux
- Dispositivo de leitura OBD-II ELM327 (USB ou Bluetooth)
- Python 2.7.14
- Qt 5.11
- [Python-OBD](https://python-obd.readthedocs.io/en/latest/): `$ pip install obd`

É necessário atentar-se ao tipo de comunicação do dispositivo de leitura ELM327, caso seja Bluetooth, como o utilizado neste trabalho, deve-se garantir o módulo de comunicação Bluetooth no computador e seguir os passos de configuração adicionais que serão listados abaixo. Ainda, caso o adaptador seja USB, a porta será do tipo *tty* e, caso seja Bluetooth, deverá ser do tipo *RFCOMM*.

##### Conexão Bluetooth
Instale os pacotes<br />
`$ sudo apt-get install bluetooth bluez-utils blueman`

Para conectar com o dispostivo:<br />
`$ hcitool scan`<br />
`$ sudo rfcomm bind 0 [endereço do adaptador OBDII]`<br />
O primeiro comando irá gerar uma lista com os dispositivos Bluetooth encontrados e seus endereços - como *"AA:BB:CC:11:22:33 - OBDII"*. Este endereço deverá ser anexado ao segundo comando, como descrito. Este comando irá gerar um driver para o dispositivo ELM327 no diretório */dev* e, caso não seja possível utilizar o número 0, pode-se tentar com outros (1, 2, 3, ...).<br />
Após conectado, o comando abaixo deverá mostrar todos os dispositivos conectados ao computador via porta *RFCOMM*:<br />
`$ ls /dev | grep rfcomm`

### OBD Comandos
O diretrio contém um programa em Python que retorna uma lista com os comandos aceitos por um veículo.

### OBData
O diretório contém a aplicação principal.


### Créditos
Lucas de Camargo Souza<br />
Edgard Haenisch Porto<br />
Ester Cavalheiro
