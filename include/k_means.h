/*
  struct que guarda as informações de cada amostra
  float x -> coordenada x
  float y -> coordenada y
  short int cluster -> cluster a que está atribuido
*/
struct point {
  float x;
  float y;
  short int cluster;
};

/*
  struct que guarda as informações do centroide de cada cluster
  float x -> coordenada x
  float y -> coordenada y
*/
struct cluster {
  float x;
  float y;
};