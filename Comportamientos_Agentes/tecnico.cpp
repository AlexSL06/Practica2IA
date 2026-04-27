#include "tecnico.hpp"
#include "motorlib/util.h"
#include <iostream>
#include <queue>
#include <set>

using namespace std;

// =========================================================================
// ÁREA DE IMPLEMENTACIÓN DEL ESTUDIANTE
// =========================================================================

Action ComportamientoTecnico::think(Sensores sensores)
{
  Action accion = IDLE;

  switch (sensores.nivel) {
    case 0: accion = ComportamientoTecnicoNivel_0(sensores); break;
    case 1: accion = ComportamientoTecnicoNivel_1(sensores); break;
    case 2: accion = ComportamientoTecnicoNivel_2(sensores); break;
    case 3: accion = ComportamientoTecnicoNivel_E(sensores); break;
    //case 3: accion = ComportamientoTecnicoNivel_3(sensores); break;
    case 4: accion = ComportamientoTecnicoNivel_4(sensores); break;
    case 5: accion = ComportamientoTecnicoNivel_5(sensores); break;
    case 6: accion = ComportamientoTecnicoNivel_6(sensores); break;
  }

  return accion;
}

int ComportamientoTecnico::veoCasillaInteresanteT0(char i, char c, char d, bool tiene_zapatillas)
{
  if (c == 'U') 
    return 2;
  else if (d == 'U') 
    return 3;
  else if (i == 'U') 
    return 1;

  if (c == 'C' || c == 'D') 
    return 2;
  else if (d == 'C' || d == 'D') 
    return 3;
  else if (i == 'C' || i == 'D') 
    return 1;

  return 0;
} 

int ComportamientoTecnico::veoCasillaInteresanteT1(char i, char c, char d, bool tiene_zapatillas)
{
  if (!tiene_zapatillas) {
    if (c == 'D') return 2;
    else if (d == 'D') return 3;
    else if (i == 'D') return 1;
  }
  if (c == 'C') return 2;
  else if (d == 'C') return 3;
  else if (i == 'C') return 1;
  else if (c == 'U') return 2;
  else if (d == 'U') return 3;
  else if (i == 'U') return 1;
  else if (c == 'S') return 2;
  else if (d == 'S') return 3;
  else if (i == 'S') return 1;
  else if (c == 'H') return 2;
  else if (d == 'H') return 3;
  else if (i == 'H') return 1;

  return 0;
}

char ComportamientoTecnico::viablePorAlturaT(char casilla, int dif)
{
  if (abs(dif) <= 1) return casilla;
  else return 'P';
}

// =========================================================================
// NIVEL 0
// =========================================================================
Action ComportamientoTecnico::ComportamientoTecnicoNivel_0(Sensores sensores)
{
  Action accion;

  ActualizarMapa(sensores);

  if (sensores.superficie[0] == 'D') tiene_zapatillas = true;

  if (visitas.empty()) {
    visitas.assign(mapaResultado.size(), vector<int>(mapaResultado[0].size(), 0));
  }
  visitas[sensores.posF][sensores.posC]++;

  ubicacion actual = {sensores.posF, sensores.posC, sensores.rumbo};
  ubicacion pos_frente = Delante(actual);

  ubicacion aux_izq = actual;
  aux_izq.brujula = (Orientacion)(((int)aux_izq.brujula + 7) % 8);
  ubicacion pos_izq = Delante(aux_izq);

  ubicacion aux_der = actual;
  aux_der.brujula = (Orientacion)(((int)aux_der.brujula + 1) % 8);
  ubicacion pos_der = Delante(aux_der);

  if (last_action == WALK &&
      sensores.posF == last_f && sensores.posC == last_c) {
    en_bloqueo = true;
    last_action = giro_preferido;
    return giro_preferido;
  }

  char i = viablePorAlturaT(sensores.superficie[1], sensores.cota[1] - sensores.cota[0]);
  char c = viablePorAlturaT(sensores.superficie[2], sensores.cota[2] - sensores.cota[0]);
  char d = viablePorAlturaT(sensores.superficie[3], sensores.cota[3] - sensores.cota[0]);

  int pos = veoCasillaInteresanteT0(i, c, d, tiene_zapatillas);

  if (sensores.superficie[0] == 'U') {
    if (c == 'U') return WALK;
    else if (d == 'U') return TURN_SR;
    else if (i == 'U') return TURN_SL;
    else return IDLE;
  }

  if (en_bloqueo && c == 'U') {
    en_bloqueo = false;
    en_bloqueo_U = true;
    contador_giros++;
    last_action = giro_preferido;
    return giro_preferido;
  }

  if (en_bloqueo_U) {
    if (pos == 2) {
      en_bloqueo_U = false;
      accion = WALK;
    }
    else {
      contador_giros++;
      accion = giro_preferido;
    }
    last_action = accion;
    return accion;
  }

  if (en_bloqueo) {
    if (pos == 2) {
      en_bloqueo = false;
      accion = WALK;
    }
    else accion = giro_preferido;
    last_action = accion;
    return accion;
  }
  else {
    if (c == 'U') { accion = WALK; giro_defecto = false; }
    else if (d == 'U') { accion = TURN_SR; giro_defecto = false; }
    else if (i == 'U') { accion = TURN_SL; giro_defecto = false; }
    else if (es_camino(sensores.superficie[1]) && (i != 'P') &&
             visitas[pos_izq.f][pos_izq.c] < visitas[pos_frente.f][pos_frente.c]) {
      accion = TURN_SL;
    }
    else if (es_camino(sensores.superficie[3]) && (d != 'P') &&
             visitas[pos_der.f][pos_der.c] < visitas[pos_frente.f][pos_frente.c]) {
      accion = TURN_SR;
    }
    else if (pos == 2) { accion = WALK; giro_defecto = false; }
    else if (pos == 3) { accion = TURN_SR; giro_defecto = false; }
    else if (pos == 1) { accion = TURN_SL; giro_defecto = false; }
    else {
      if (!giro_defecto) {
        accion = giro_preferido;
        giro_defecto = true;
      }
      else if (giro_preferido == TURN_SL) accion = TURN_SR;
      else accion = TURN_SL;
      contador_giros++;
    }

    if (contador_giros >= 15) {
      giro_preferido = (giro_preferido == TURN_SL) ? TURN_SR : TURN_SL;
      contador_giros = 0;
    }
  }

  if (accion == WALK) {
    cont_walk++;
    if (cont_walk >= 5) {
      if (walk_left) { accion = TURN_SL; walk_left = false; }
      else { accion = TURN_SR; walk_left = true; }
      cont_walk = 0;
      contador_giros++;
    }
  }
  else cont_walk = 0;

  last_f = sensores.posF;
  last_c = sensores.posC;
  last_action = accion;

  return accion;
}

bool ComportamientoTecnico::es_camino(unsigned char c) const
{
  return (c == 'C' || c == 'D' || c == 'U');
}

bool ComportamientoTecnico::es_camino1(unsigned char c) const
{
  return (c == 'C' || c == 'D' || c == 'U' || c == 'S' || c == 'H');
}

/**
 * @brief Comportamiento reactivo del técnico para el Nivel 1.
 * @param sensores Datos actuales de los sensores.
 * @return Acción a realizar.
 */
Action ComportamientoTecnico::ComportamientoTecnicoNivel_1(Sensores sensores)
{
  Action accion;

  ActualizarMapa(sensores);

  if (sensores.superficie[0] == 'D') tiene_zapatillas = true;

  if (visitas.empty()) {
    visitas.assign(mapaResultado.size(), vector<int>(mapaResultado[0].size(), 0));
  }
  visitas[sensores.posF][sensores.posC]++;

  ubicacion actual = {sensores.posF, sensores.posC, sensores.rumbo};
  ubicacion pos_frente = Delante(actual);

  ubicacion aux_izq = actual;
  aux_izq.brujula = (Orientacion)(((int)aux_izq.brujula + 7) % 8);
  ubicacion pos_izq = Delante(aux_izq);

  ubicacion aux_der = actual;
  aux_der.brujula = (Orientacion)(((int)aux_der.brujula + 1) % 8);
  ubicacion pos_der = Delante(aux_der);

  if (last_action == WALK &&
      sensores.posF == last_f && sensores.posC == last_c) {
    en_bloqueo = true;
    last_action = giro_preferido;
    return giro_preferido;
  }

  char i = viablePorAlturaT(sensores.superficie[1], sensores.cota[1] - sensores.cota[0]);
  char c = viablePorAlturaT(sensores.superficie[2], sensores.cota[2] - sensores.cota[0]);
  char d = viablePorAlturaT(sensores.superficie[3], sensores.cota[3] - sensores.cota[0]);

  int pos = veoCasillaInteresanteT1(i, c, d, tiene_zapatillas);

  if (en_bloqueo) {
    if (sensores.superficie[2] != 'D' && es_camino1(sensores.superficie[2]) && sensores.superficie[2] == c) {
      en_bloqueo = false;
      accion = WALK;
    }
    else accion = giro_preferido;
    last_action = accion;
    return accion;
  }
  else {
    if (es_camino1(sensores.superficie[1]) && (i != 'A') && (i != 'P') &&
        visitas[pos_izq.f][pos_izq.c] < visitas[pos_frente.f][pos_frente.c]) {
      accion = TURN_SL;
    }
    else if (es_camino1(sensores.superficie[3]) && (d != 'A') && (d != 'P') &&
             visitas[pos_der.f][pos_der.c] < visitas[pos_frente.f][pos_frente.c]) {
      accion = TURN_SR;
    }
    else if (es_camino1(sensores.superficie[2]) && (c != 'A') && (c != 'P') && sensores.superficie[2] == c) {
      accion = WALK;
    }
    else if (pos == 2) { accion = WALK; giro_defecto = false; }
    else if (pos == 3) { accion = TURN_SR; giro_defecto = false; }
    else if (pos == 1) { accion = TURN_SL; giro_defecto = false; }
    else {
      if (!giro_defecto) {
        accion = giro_preferido;
        giro_defecto = true;
      }
      else if (giro_preferido == TURN_SL) accion = TURN_SR;
      else accion = TURN_SL;
      contador_giros++;
    }

    if (contador_giros >= 15) {
      giro_preferido = (giro_preferido == TURN_SL) ? TURN_SR : TURN_SL;
      contador_giros = 0;
    }
  }

  if (accion == WALK) {
    cont_walk++;
    if (cont_walk >= 4) {
      if (walk_left) { accion = TURN_SL; walk_left = false; }
      else { accion = TURN_SR; walk_left = true; }
      cont_walk = 0;
      contador_giros++;
    }
  }
  else cont_walk = 0;

  last_f = sensores.posF;
  last_c = sensores.posC;
  last_action = accion;

  return accion;
}
//niveles 2 3 4 5 6 

list<Action> BREADTH_1ST_SEARCH(){
  list<Action> secuencia;
  secuencia.push_back(WALK);
  secuencia.push_back(WALK);
  secuencia.push_back(TURN_SR);
  secuencia.push_back(TURN_SR);
  secuencia.push_back(WALK);
  return secuencia;
}

EstadoT NextCasillaTécnico(const EstadoT &st){
  EstadoT siguiente = st;
  switch (st.site.brujula)
  {
  case norte:
    siguiente.site.f = st.site.f - 1;
    break;
  case noreste:
    siguiente.site.f = st.site.f - 1;
    siguiente.site.c = st.site.c + 1;
    break;
  case este:
    siguiente.site.c = st.site.c + 1;
    break;
  case sureste:
    siguiente.site.f = st.site.f + 1;
    siguiente.site.c = st.site.c + 1;
    break;
  case sur:
    siguiente.site.f = st.site.f + 1;
    break;
  case suroeste:
    siguiente.site.f = st.site.f + 1;
    siguiente.site.c = st.site.c - 1;
    break;
  case oeste:
    siguiente.site.c = st.site.c - 1;
    break;
  case noroeste:
    siguiente.site.f = st.site.f - 1;
    siguiente.site.c = st.site.c - 1;
  }
  return siguiente;
}

bool CasillaAccesibleTécnico(const EstadoT &st, const vector<vector<unsigned char>> &terreno, const
vector<vector<unsigned char>> &altura){
  EstadoT next = NextCasillaTécnico(st);
  bool check1 = false, check2 = false, check3 = false;
  check1 = terreno[next.site.f][next.site.c] != 'P' and terreno[next.site.f][next.site.c] != 'M';
  check2 = terreno[next.site.f][next.site.c] != 'B' or (terreno[next.site.f][next.site.c] == 'B' and st.zapatillas);
  check3 = abs(altura[next.site.f][next.site.c] - altura[st.site.f][st.site.c]) <= 1;
  return check1 and check2 and check3;
}
EstadoT applyT(Action accion, const EstadoT & st, const vector<vector<unsigned char>> &terreno, const
vector<vector<unsigned char>> &altura){
  EstadoT next = st;
  switch(accion){
    case WALK:
      if (CasillaAccesibleTécnico(st,terreno,altura)){
        next = NextCasillaTécnico(st);
      }
      break;
    case TURN_SR:
      next.site.brujula = (Orientacion) ((next.site.brujula+1)%8);
      break;
    case TURN_SL:
      next.site.brujula = (Orientacion) ((next.site.brujula+7)%8);
      break;
    }
    return next;
}

bool Find (const NodoT & st, const list<NodoT> &lista){
  auto it = lista.begin();
  while (it != lista.end() and !((*it) == st)){
    it++;
  }
  return (it != lista.end());
}
/**
 * @brief Primera aproximación a la búsqueda en anchura
 * * @param inicio Estado Inicial de la búsqueda.
 * @param final Estado Fianl de la búsqueda.
 * @param terreno Matriz que contiene la información del terreno.
 * @param altura Matriz que contiene la altura del mapa.
 * * @return La secuencia de acciones para llegar al estado final.
 * @note Devuelve un plan vacío si no es posible encontrar un plan válido.
 */
list<Action> ComportamientoTecnico::B_Anchura(const EstadoT &inicio, const EstadoT &final,
                                              const vector<vector<unsigned char>> &terreno,
                                              const vector<vector<unsigned char>> &altura){
  NodoT current_node; //almacena el estado actual y que inicialmente toma el valor del parámetro inicio.
  list<NodoT> frontier;//una lista que mantiene los nodos pendientes de explorar, inicialmente vacía.
  list<NodoT> explored;//una lista que mantiene los nodos ya explorados, inicialmente vacía.
  list<Action> path;//variable que devolverá la secuencia de acciones encontrada como solución.

  current_node.estado = inicio;
  frontier.push_back(current_node);
  bool SolutionFound = (current_node.estado.site.f == final.site.f and current_node.estado.site.c == final.site.c);
  /*una variable lógica que determina si ya se ha encontrado un estado que satisface la condición de ser solución. Esta variable se inicializa mirando si current_note
  satisface la condición de ser solución, es decir, que el agente jugador esté en la fila y en la columna de la casilla destino*/
  while (!SolutionFound and !frontier.empty()){
    frontier.pop_front();
    explored.push_back(current_node);

    // Compruebo si estoy en una casilla que da las zapatillas
    if (terreno[current_node.estado.site.f][current_node.estado.site.c] == 'D'){
      current_node.estado.zapatillas = true;
    }

    // Genero el hijo resultante de aplicar la acción WALK
    NodoT child_Walk = current_node;
    child_Walk.estado = applyT(WALK, current_node.estado, terreno, altura);
    if (child_Walk.estado.site.f == final.site.f and child_Walk.estado.site.c == final.site.c){
      // El hijo generado es solucion
      child_Walk.secuencia.push_back(WALK);
      current_node = child_Walk;
      SolutionFound = true;
    }
    else if (!Find(child_Walk, frontier) and !Find(child_Walk, explored)){
      // Se mete en la lista de frontier después de añadir a secuencia la acción
      child_Walk.secuencia.push_back(WALK);
      frontier.push_back(child_Walk);
    }

    if (!SolutionFound){
      // El hijo resultante de aplicar la accion TURN_SR
      NodoT child_TurnSR = current_node;
      child_TurnSR.estado = applyT(TURN_SR, current_node.estado, terreno, altura);
      if (!Find(child_TurnSR, frontier) and !Find(child_TurnSR, explored)){
        child_TurnSR.secuencia.push_back(TURN_SR);
        frontier.push_back(child_TurnSR);
      }

      // El hijo resultante de aplicar la accion TURN_SL
      NodoT child_TurnSL = current_node;
      child_TurnSL.estado = applyT(TURN_SL, current_node.estado, terreno, altura);
      if (!Find(child_TurnSL, frontier) and !Find(child_TurnSL, explored)){
        child_TurnSL.secuencia.push_back(TURN_SL);
        frontier.push_back(child_TurnSL);
      }
    }

    // Paso a evaluar el siguiente nodo en la lista "frontier"
    if (!SolutionFound and !frontier.empty()){
      current_node = frontier.front();
      SolutionFound = (current_node.estado.site.f == final.site.f and current_node.estado.site.c == final.site.c);
    }
  }
  // Devuelvo el camino encontrado.
  if (SolutionFound)
    path = current_node.secuencia;
  return path;

}
/**
 * @brief Segundoa aproximación a la búsqueda en anchura
 * * @param inicio Estado Inicial de la búsqueda.
 * @param final Estado Fianl de la búsqueda.
 * @param terreno Matriz que contiene la información del terreno.
 * @param altura Matriz que contiene la altura del mapa.
 * * @return La secuencia de acciones para llegar al estado final.
 * @note Devuelve un plan vacío si no es posible encontrar un plan válido.
 * @note Explored pasa a ser implementado mediante un "set" en lugar de un "list"
 */
list<Action> ComportamientoTecnico::B_Anchura_V2(const EstadoT &inicio, const EstadoT &final,
                                                 const vector<vector<unsigned char>> &terreno,
                                                 const vector<vector<unsigned char>> &altura){
  NodoT current_node;
  list<NodoT> frontier;
  set<NodoT> explored;
  list<Action> path;

  current_node.estado = inicio;
  frontier.push_back(current_node);
  bool SolutionFound = (current_node.estado.site.f == final.site.f and current_node.estado.site.c == final.site.c);
  while (!SolutionFound && !frontier.empty()) {
    frontier.pop_front();
    explored.insert(current_node);

    // Compruebo si estoy en una casilla que da las zapatillas
    if (terreno[current_node.estado.site.f][current_node.estado.site.c] == 'D') {
        current_node.estado.zapatillas = true;
    }

    // Genero el hijo resultante de aplicar la acción WALK
    NodoT childWalk = current_node;
    childWalk.estado = applyT(WALK, current_node.estado, terreno, altura);
    if (childWalk.estado.site.f == final.site.f && childWalk.estado.site.c == final.site.c) {
        // El hijo generado es solución
        childWalk.secuencia.push_back(WALK);
        current_node = childWalk;
        SolutionFound = true;
    } else if (explored.find(childWalk) == explored.end()) {
        // Se mete en la lista frontier después de añadir a secuencia la acción
        childWalk.secuencia.push_back(WALK);
        frontier.push_back(childWalk);
    }

    if (!SolutionFound) {
        // El hijo resultante de aplicar la acción TURN_SR
        NodoT childTurnSR = current_node;
        childTurnSR.estado = applyT(TURN_SR, current_node.estado, terreno, altura);
        if (explored.find(childTurnSR) == explored.end()) {
            childTurnSR.secuencia.push_back(TURN_SR);
            frontier.push_back(childTurnSR);
        }

        // El hijo resultante de aplicar la acción TURN_SL
        NodoT childTurnSL = current_node;
        childTurnSL.estado = applyT(TURN_SL, current_node.estado, terreno, altura);
        if (explored.find(childTurnSL) == explored.end()) {
            childTurnSL.secuencia.push_back(TURN_SL);
            frontier.push_back(childTurnSL);
        }
    }

    // Paso a evaluar el siguiente nodo en la lista "frontier"
    if (!SolutionFound && !frontier.empty()) {
        current_node = frontier.front();
        while (explored.find(current_node) != explored.end() && !frontier.empty()) {
            frontier.pop_front();
            current_node = frontier.front();
        }
    }
  }
  // Devuelvo el camino encontrado.
  if (SolutionFound)
    path = current_node.secuencia;

  return path;
}
Action ComportamientoTecnico::ComportamientoTecnicoNivel_E(Sensores sensores){
  Action accion = IDLE;
  if (!hayPlan){
    // Invocar al método de búsqueda
    EstadoT inicio, fin;
    inicio.site.f = sensores.posF;
    inicio.site.c = sensores.posC;
    inicio.site.brujula = sensores.rumbo;
    inicio.zapatillas = tiene_zapatillas;
    fin.site.f = sensores.BelPosF;
    fin.site.c = sensores.BelPosC;
    plan = B_Anchura_V2(inicio, fin, mapaResultado, mapaCotas);
    VisualizaPlan(inicio.site,plan);
    hayPlan = plan.size() != 0 ;
  }
  if (hayPlan and plan.size()>0){
    accion = plan.front();
    plan.pop_front();
  }
  if (plan.size()== 0){
    hayPlan = false;
  }
  return accion;
}

/**
 * @brief Comportamiento del técnico para el Nivel 2.
 * @param sensores Datos actuales de los sensores.
 * @return Acción a realizar.
 */
Action ComportamientoTecnico::ComportamientoTecnicoNivel_2(Sensores sensores) {
  return IDLE;
}

/**
 * @brief Comportamiento del técnico para el Nivel 3.
 * @param sensores Datos actuales de los sensores.
 * @return Acción a realizar.
 */
Action ComportamientoTecnico::ComportamientoTecnicoNivel_3(Sensores sensores) {
  return IDLE;
}

/**
 * @brief Comportamiento del técnico para el Nivel 4.
 * @param sensores Datos actuales de los sensores.
 * @return Acción a realizar.
 */
Action ComportamientoTecnico::ComportamientoTecnicoNivel_4(Sensores sensores) {
  return IDLE;
}

/**
 * @brief Comportamiento del técnico para el Nivel 5.
 * @param sensores Datos actuales de los sensores.
 * @return Acción a realizar.
 */
Action ComportamientoTecnico::ComportamientoTecnicoNivel_5(Sensores sensores) {
  return IDLE;
}

/**
 * @brief Comportamiento del técnico para el Nivel 6.
 * @param sensores Datos actuales de los sensores.
 * @return Acción a realizar.
 */
Action ComportamientoTecnico::ComportamientoTecnicoNivel_6(Sensores sensores) {
  return IDLE;
}



// =========================================================================
// FUNCIONES PROPORCIONADAS
// =========================================================================

/**
 * @brief Actualiza el mapaResultado y mapaCotas con la información de los sensores.
 * @param sensores Datos actuales de los sensores.
 */
void ComportamientoTecnico::ActualizarMapa(Sensores sensores) {
  mapaResultado[sensores.posF][sensores.posC] = sensores.superficie[0];
  mapaCotas[sensores.posF][sensores.posC] = sensores.cota[0];

  int pos = 1;
  switch (sensores.rumbo) {
    case norte:
      for (int j = 1; j < 4; j++)
        for (int i = -j; i <= j; i++) {
          mapaResultado[sensores.posF - j][sensores.posC + i] = sensores.superficie[pos];
          mapaCotas[sensores.posF - j][sensores.posC + i] = sensores.cota[pos++];
        }
      break;
    case noreste:
      mapaResultado[sensores.posF - 1][sensores.posC] = sensores.superficie[1];
      mapaCotas[sensores.posF - 1][sensores.posC] = sensores.cota[1];
      mapaResultado[sensores.posF - 1][sensores.posC + 1] = sensores.superficie[2];
      mapaCotas[sensores.posF - 1][sensores.posC + 1] = sensores.cota[2];
      mapaResultado[sensores.posF][sensores.posC + 1] = sensores.superficie[3];
      mapaCotas[sensores.posF][sensores.posC + 1] = sensores.cota[3];
      mapaResultado[sensores.posF - 2][sensores.posC] = sensores.superficie[4];
      mapaCotas[sensores.posF - 2][sensores.posC] = sensores.cota[4];
      mapaResultado[sensores.posF - 2][sensores.posC + 1] = sensores.superficie[5];
      mapaCotas[sensores.posF - 2][sensores.posC + 1] = sensores.cota[5];
      mapaResultado[sensores.posF - 2][sensores.posC + 2] = sensores.superficie[6];
      mapaCotas[sensores.posF - 2][sensores.posC + 2] = sensores.cota[6];
      mapaResultado[sensores.posF - 1][sensores.posC + 2] = sensores.superficie[7];
      mapaCotas[sensores.posF - 1][sensores.posC + 2] = sensores.cota[7];
      mapaResultado[sensores.posF][sensores.posC + 2] = sensores.superficie[8];
      mapaCotas[sensores.posF][sensores.posC + 2] = sensores.cota[8];
      mapaResultado[sensores.posF - 3][sensores.posC] = sensores.superficie[9];
      mapaCotas[sensores.posF - 3][sensores.posC] = sensores.cota[9];
      mapaResultado[sensores.posF - 3][sensores.posC + 1] = sensores.superficie[10];
      mapaCotas[sensores.posF - 3][sensores.posC + 1] = sensores.cota[10];
      mapaResultado[sensores.posF - 3][sensores.posC + 2] = sensores.superficie[11];
      mapaCotas[sensores.posF - 3][sensores.posC + 2] = sensores.cota[11];
      mapaResultado[sensores.posF - 3][sensores.posC + 3] = sensores.superficie[12];
      mapaCotas[sensores.posF - 3][sensores.posC + 3] = sensores.cota[12];
      mapaResultado[sensores.posF - 2][sensores.posC + 3] = sensores.superficie[13];
      mapaCotas[sensores.posF - 2][sensores.posC + 3] = sensores.cota[13];
      mapaResultado[sensores.posF - 1][sensores.posC + 3] = sensores.superficie[14];
      mapaCotas[sensores.posF - 1][sensores.posC + 3] = sensores.cota[14];
      mapaResultado[sensores.posF][sensores.posC + 3] = sensores.superficie[15];
      mapaCotas[sensores.posF][sensores.posC + 3] = sensores.cota[15];
      break;
    case este:
      for (int j = 1; j < 4; j++)
        for (int i = -j; i <= j; i++) {
          mapaResultado[sensores.posF + i][sensores.posC + j] = sensores.superficie[pos];
          mapaCotas[sensores.posF + i][sensores.posC + j] = sensores.cota[pos++];
        }
      break;
    case sureste:
      mapaResultado[sensores.posF][sensores.posC + 1] = sensores.superficie[1];
      mapaCotas[sensores.posF][sensores.posC + 1] = sensores.cota[1];
      mapaResultado[sensores.posF + 1][sensores.posC + 1] = sensores.superficie[2];
      mapaCotas[sensores.posF + 1][sensores.posC + 1] = sensores.cota[2];
      mapaResultado[sensores.posF + 1][sensores.posC] = sensores.superficie[3];
      mapaCotas[sensores.posF + 1][sensores.posC] = sensores.cota[3];
      mapaResultado[sensores.posF][sensores.posC + 2] = sensores.superficie[4];
      mapaCotas[sensores.posF][sensores.posC + 2] = sensores.cota[4];
      mapaResultado[sensores.posF + 1][sensores.posC + 2] = sensores.superficie[5];
      mapaCotas[sensores.posF + 1][sensores.posC + 2] = sensores.cota[5];
      mapaResultado[sensores.posF + 2][sensores.posC + 2] = sensores.superficie[6];
      mapaCotas[sensores.posF + 2][sensores.posC + 2] = sensores.cota[6];
      mapaResultado[sensores.posF + 2][sensores.posC + 1] = sensores.superficie[7];
      mapaCotas[sensores.posF + 2][sensores.posC + 1] = sensores.cota[7];
      mapaResultado[sensores.posF + 2][sensores.posC] = sensores.superficie[8];
      mapaCotas[sensores.posF + 2][sensores.posC] = sensores.cota[8];
      mapaResultado[sensores.posF][sensores.posC + 3] = sensores.superficie[9];
      mapaCotas[sensores.posF][sensores.posC + 3] = sensores.cota[9];
      mapaResultado[sensores.posF + 1][sensores.posC + 3] = sensores.superficie[10];
      mapaCotas[sensores.posF + 1][sensores.posC + 3] = sensores.cota[10];
      mapaResultado[sensores.posF + 2][sensores.posC + 3] = sensores.superficie[11];
      mapaCotas[sensores.posF + 2][sensores.posC + 3] = sensores.cota[11];
      mapaResultado[sensores.posF + 3][sensores.posC + 3] = sensores.superficie[12];
      mapaCotas[sensores.posF + 3][sensores.posC + 3] = sensores.cota[12];
      mapaResultado[sensores.posF + 3][sensores.posC + 2] = sensores.superficie[13];
      mapaCotas[sensores.posF + 3][sensores.posC + 2] = sensores.cota[13];
      mapaResultado[sensores.posF + 3][sensores.posC + 1] = sensores.superficie[14];
      mapaCotas[sensores.posF + 3][sensores.posC + 1] = sensores.cota[14];
      mapaResultado[sensores.posF + 3][sensores.posC] = sensores.superficie[15];
      mapaCotas[sensores.posF + 3][sensores.posC] = sensores.cota[15];
      break;
    case sur:
      for (int j = 1; j < 4; j++)
        for (int i = -j; i <= j; i++) {
          mapaResultado[sensores.posF + j][sensores.posC - i] = sensores.superficie[pos];
          mapaCotas[sensores.posF + j][sensores.posC - i] = sensores.cota[pos++];
        }
      break;
    case suroeste:
      mapaResultado[sensores.posF + 1][sensores.posC] = sensores.superficie[1];
      mapaCotas[sensores.posF + 1][sensores.posC] = sensores.cota[1];
      mapaResultado[sensores.posF + 1][sensores.posC - 1] = sensores.superficie[2];
      mapaCotas[sensores.posF + 1][sensores.posC - 1] = sensores.cota[2];
      mapaResultado[sensores.posF][sensores.posC - 1] = sensores.superficie[3];
      mapaCotas[sensores.posF][sensores.posC - 1] = sensores.cota[3];
      mapaResultado[sensores.posF + 2][sensores.posC] = sensores.superficie[4];
      mapaCotas[sensores.posF + 2][sensores.posC] = sensores.cota[4];
      mapaResultado[sensores.posF + 2][sensores.posC - 1] = sensores.superficie[5];
      mapaCotas[sensores.posF + 2][sensores.posC - 1] = sensores.cota[5];
      mapaResultado[sensores.posF + 2][sensores.posC - 2] = sensores.superficie[6];
      mapaCotas[sensores.posF + 2][sensores.posC - 2] = sensores.cota[6];
      mapaResultado[sensores.posF + 1][sensores.posC - 2] = sensores.superficie[7];
      mapaCotas[sensores.posF + 1][sensores.posC - 2] = sensores.cota[7];
      mapaResultado[sensores.posF][sensores.posC - 2] = sensores.superficie[8];
      mapaCotas[sensores.posF][sensores.posC - 2] = sensores.cota[8];
      mapaResultado[sensores.posF + 3][sensores.posC] = sensores.superficie[9];
      mapaCotas[sensores.posF + 3][sensores.posC] = sensores.cota[9];
      mapaResultado[sensores.posF + 3][sensores.posC - 1] = sensores.superficie[10];
      mapaCotas[sensores.posF + 3][sensores.posC - 1] = sensores.cota[10];
      mapaResultado[sensores.posF + 3][sensores.posC - 2] = sensores.superficie[11];
      mapaCotas[sensores.posF + 3][sensores.posC - 2] = sensores.cota[11];
      mapaResultado[sensores.posF + 3][sensores.posC - 3] = sensores.superficie[12];
      mapaCotas[sensores.posF + 3][sensores.posC - 3] = sensores.cota[12];
      mapaResultado[sensores.posF + 2][sensores.posC - 3] = sensores.superficie[13];
      mapaCotas[sensores.posF + 2][sensores.posC - 3] = sensores.cota[13];
      mapaResultado[sensores.posF + 1][sensores.posC - 3] = sensores.superficie[14];
      mapaCotas[sensores.posF + 1][sensores.posC - 3] = sensores.cota[14];
      mapaResultado[sensores.posF][sensores.posC - 3] = sensores.superficie[15];
      mapaCotas[sensores.posF][sensores.posC - 3] = sensores.cota[15];
      break;
    case oeste:
      for (int j = 1; j < 4; j++)
        for (int i = -j; i <= j; i++) {
          mapaResultado[sensores.posF - i][sensores.posC - j] = sensores.superficie[pos];
          mapaCotas[sensores.posF - i][sensores.posC - j] = sensores.cota[pos++];
        }
      break;
    case noroeste:
      mapaResultado[sensores.posF][sensores.posC - 1] = sensores.superficie[1];
      mapaCotas[sensores.posF][sensores.posC - 1] = sensores.cota[1];
      mapaResultado[sensores.posF - 1][sensores.posC - 1] = sensores.superficie[2];
      mapaCotas[sensores.posF - 1][sensores.posC - 1] = sensores.cota[2];
      mapaResultado[sensores.posF - 1][sensores.posC] = sensores.superficie[3];
      mapaCotas[sensores.posF - 1][sensores.posC] = sensores.cota[3];
      mapaResultado[sensores.posF][sensores.posC - 2] = sensores.superficie[4];
      mapaCotas[sensores.posF][sensores.posC - 2] = sensores.cota[4];
      mapaResultado[sensores.posF - 1][sensores.posC - 2] = sensores.superficie[5];
      mapaCotas[sensores.posF - 1][sensores.posC - 2] = sensores.cota[5];
      mapaResultado[sensores.posF - 2][sensores.posC - 2] = sensores.superficie[6];
      mapaCotas[sensores.posF - 2][sensores.posC - 2] = sensores.cota[6];
      mapaResultado[sensores.posF - 2][sensores.posC - 1] = sensores.superficie[7];
      mapaCotas[sensores.posF - 2][sensores.posC - 1] = sensores.cota[7];
      mapaResultado[sensores.posF - 2][sensores.posC] = sensores.superficie[8];
      mapaCotas[sensores.posF - 2][sensores.posC] = sensores.cota[8];
      mapaResultado[sensores.posF][sensores.posC - 3] = sensores.superficie[9];
      mapaCotas[sensores.posF][sensores.posC - 3] = sensores.cota[9];
      mapaResultado[sensores.posF - 1][sensores.posC - 3] = sensores.superficie[10];
      mapaCotas[sensores.posF - 1][sensores.posC - 3] = sensores.cota[10];
      mapaResultado[sensores.posF - 2][sensores.posC - 3] = sensores.superficie[11];
      mapaCotas[sensores.posF - 2][sensores.posC - 3] = sensores.cota[11];
      mapaResultado[sensores.posF - 3][sensores.posC - 3] = sensores.superficie[12];
      mapaCotas[sensores.posF - 3][sensores.posC - 3] = sensores.cota[12];
      mapaResultado[sensores.posF - 3][sensores.posC - 2] = sensores.superficie[13];
      mapaCotas[sensores.posF - 3][sensores.posC - 2] = sensores.cota[13];
      mapaResultado[sensores.posF - 3][sensores.posC - 1] = sensores.superficie[14];
      mapaCotas[sensores.posF - 3][sensores.posC - 1] = sensores.cota[14];
      mapaResultado[sensores.posF - 3][sensores.posC] = sensores.superficie[15];
      mapaCotas[sensores.posF - 3][sensores.posC] = sensores.cota[15];
      break;
  }
}



/**
 * @brief Determina si una casilla es transitable para el técnico.
 * En esta práctica, si el técnico tiene zapatillas, el bosque ('B') es transitable.
 * @param f Fila de la casilla.
 * @param c Columna de la casilla.
 * @param tieneZapatillas Indica si el agente posee las zapatillas.
 * @return true si la casilla es transitable.
 */
bool ComportamientoTecnico::EsCasillaTransitableLevel0(int f, int c, bool tieneZapatillas) {
  if (f < 0 || f >= mapaResultado.size() || c < 0 || c >= mapaResultado[0].size()) return false;
  return es_camino(mapaResultado[f][c]);  // Solo 'C', 'S', 'D', 'U' son transitables en Nivel 0
}

/**
 * @brief Comprueba si la casilla de delante es accesible por diferencia de altura.
 * Para el técnico: desnivel máximo siempre 1.
 * @param actual Estado actual del agente (fila, columna, orientacion).
 * @return true si el desnivel con la casilla de delante es admisible.
 */
bool ComportamientoTecnico::EsAccesiblePorAltura(const ubicacion &actual) {
  ubicacion del = Delante(actual);
  if (del.f < 0 || del.f >= mapaCotas.size() || del.c < 0 || del.c >= mapaCotas[0].size()) return false;
  int desnivel = abs(mapaCotas[del.f][del.c] - mapaCotas[actual.f][actual.c]);
  if (desnivel > 1) return false;
  return true;
}

/**
 * @brief Devuelve la posición (fila, columna) de la casilla que hay delante del agente.
 * Calcula la casilla frontal según la orientación actual (8 direcciones).
 * @param actual Estado actual del agente (fila, columna, orientacion).
 * @return Estado con la fila y columna de la casilla de enfrente.
 */
ubicacion ComportamientoTecnico::Delante(const ubicacion &actual) const {
  ubicacion delante = actual;
  switch (actual.brujula) {
    case 0: delante.f--; break;                        // norte
    case 1: delante.f--; delante.c++; break;     // noreste
    case 2: delante.c++; break;                     // este
    case 3: delante.f++; delante.c++; break;     // sureste
    case 4: delante.f++; break;                        // sur
    case 5: delante.f++; delante.c--; break;     // suroeste
    case 6: delante.c--; break;                     // oeste
    case 7: delante.f--; delante.c--; break;     // noroeste
  }
  return delante;
}


/**
 * @brief Imprime por consola la secuencia de acciones de un plan.
 *
 * @param plan  Lista de acciones del plan.
 */
void ComportamientoTecnico::PintaPlan(const list<Action> &plan)
{
  auto it = plan.begin();
  while (it != plan.end())
  {
    if (*it == WALK)
    {
      cout << "W ";
    }
    else if (*it == JUMP)
    {
      cout << "J ";
    }
    else if (*it == TURN_SR)
    {
      cout << "r ";
    }
    else if (*it == TURN_SL)
    {
      cout << "l ";
    }
    else if (*it == COME)
    {
      cout << "C ";
    }
    else if (*it == IDLE)
    {
      cout << "I ";
    }
    else
    {
      cout << "-_ ";
    }
    it++;
  }
  cout << "( longitud " << plan.size() << ")" << endl;
}



/**
 * @brief Convierte un plan de acciones en una lista de casillas para
 *        su visualización en el mapa 2D.
 *
 * @param st    Estado de partida.
 * @param plan  Lista de acciones del plan.
 */
void ComportamientoTecnico::VisualizaPlan(const ubicacion &st,
                                            const list<Action> &plan)
{
   listaPlanCasillas.clear();
  ubicacion cst = st;

  listaPlanCasillas.push_back({cst.f, cst.c, WALK});
  auto it = plan.begin();
  while (it != plan.end())
  {

    switch (*it)
    {
    case JUMP:
      switch (cst.brujula)
      {
      case 0:
        cst.f--;
        break;
      case 1:
        cst.f--;
        cst.c++;
        break;
      case 2:
        cst.c++;
        break;
      case 3:
        cst.f++;
        cst.c++;
        break;
      case 4:
        cst.f++;
        break;
      case 5:
        cst.f++;
        cst.c--;
        break;
      case 6:
        cst.c--;
        break;
      case 7:
        cst.f--;
        cst.c--;
        break;
      }
      if (cst.f >= 0 && cst.f < mapaResultado.size() &&
          cst.c >= 0 && cst.c < mapaResultado[0].size())
        listaPlanCasillas.push_back({cst.f, cst.c, JUMP});
    case WALK:
      switch (cst.brujula)
      {
      case 0:
        cst.f--;
        break;
      case 1:
        cst.f--;
        cst.c++;
        break;
      case 2:
        cst.c++;
        break;
      case 3:
        cst.f++;
        cst.c++;
        break;
      case 4:
        cst.f++;
        break;
      case 5:
        cst.f++;
        cst.c--; //OJO CORRECCION
        break;
      case 6:
        cst.c--;
        break;
      case 7:
        cst.f--;
        cst.c--;
        break;
      }
      if (cst.f >= 0 && cst.f < mapaResultado.size() &&
          cst.c >= 0 && cst.c < mapaResultado[0].size())
        listaPlanCasillas.push_back({cst.f, cst.c, WALK});
      break;
    case TURN_SR:
      cst.brujula = (Orientacion) (( (int) cst.brujula + 1) % 8);
      break;
    case TURN_SL:
      cst.brujula = (Orientacion) (( (int) cst.brujula + 7) % 8);
      break;
    }
    it++;
  }
}

