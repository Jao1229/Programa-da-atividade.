O Programa 1

Tem como objetivo realizar a organização dos dados brutos de sensores, estruturando o conteúdo do arquivo dados_sensores.txt para que possa ser utilizado de forma rápida e confiável nos programas seguintes do sistema. Esse processo envolve interpretar cada linha do arquivo original, validar os dados presentes (como o timestamp, o nome do sensor e o valor medido), separar as leituras por sensor e ordená-las em ordem decrescente com base no timestamp. Para realizar a ordenação dos dados, foi utilizada a função qsort, que oferece uma maneira eficiente de classificar os registros.

Ao final da execução, o programa gera arquivos individuais para cada sensor  encontrado no arquivo original, contendo somente os dados correspondentes àquele sensor, todos já organizados do mais recente para o mais antigo. Essa estrutura facilita a realização de buscas e análises posteriores.

Para executar o programa, basta digitar no terminal de comando: .\programa1 dados_sensores.txt. Vale destacar que, durante os testes realizados, foi observado que em alguns computadores (incluindo o do autor do código), foi necessário utilizar o prefixo .\ antes do nome do programa e, em casos onde os parâmetros envolviam números, foi preciso utilizar aspas ao redor do nome do arquivo (por exemplo: "dados_sensores_01.txt"). Em outros sistemas, no entanto, a execução ocorreu normalmente utilizando apenas o comando padrão, como ./programa1.exe dados_sensores.txt, sem a necessidade de ajustes adicionais.

Programa 2

Tem como objetivo permitir a consulta da leitura de um sensor específico, com base em uma data e hora fornecidas pelo usuário. Ele foi desenvolvido para carregar os dados de sensores a partir de um arquivo previamente organizado (gerado pelo Programa 1), interpretar os timestamps das leituras, convertê-los para um formato compreensível, e então realizar uma busca binária eficiente para localizar a medição mais próxima do instante solicitado.

A execução do programa é feita via terminal, sendo obrigatório informar dois argumentos: o nome do arquivo contendo os dados de um sensor e a data/hora desejada no formato dd/mm/aaaa-hh:mm:ss. Um exemplo de uso seria: .\programa2 dados_sensores.txt "05/02/2003-10:00:00"

O Programa 3 é responsável por gerar um arquivo de medições aleatores presente nos sensores. O usuário deve informar o nome do programa, o intervalo de datas e horas para as amostras(talvez uso de " entorno de datas e hora), os tipos de dados que cada sensor representa. O arquivo gerado terá a extensão .txt. 
Ex: .\programa3 "01/01/2001-09:00:00" "19/05/2005-17:00:00" LUX TEXTO PRESSAO
Outros termos alem de LUZ ou PRESSAO tem tambem UMID TEMP
O programa cria então 2000 leituras aleatórias para cada sensor, com timestamps gerados aleatoriamente dentro do intervalo informado, respeitando os tipos de dados especificados para cada sensor.

Ao finalizar a execução com sucesso, o programa exibe a mensagem.
