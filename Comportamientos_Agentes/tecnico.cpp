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
    case 3: accion = ComportamientoTecnicoNivel_3(sensores); break;
    //case 3: accion = ComportamientoTecnicoNivel_E(sensores); break;
    case 4: accion = ComportamientoTecnicoNivel_4(sensores); break;
    case 5: accion = ComportamientoTecnicoNivel_5(sensores); break;
    case 6: accion = ComportamientoTecnicoNivel_6(sensores); break;
  }

  return accion;
}

char ComportamientoTecnico::viablePorAlturaT(char casilla, int dif)
{
  if (abs(dif) <= 1) return casilla;
  else return 'P';
}

// =========================================================================
// NIVEL 0
// =========================================================================
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
bool ComportamientoTecnico::es_camino(unsigned char c) const
{
  return (c == 'C' || c == 'D' || c == 'U');
}

/////////////////////////////////////////////////////////
//NIVEL 1
/////////////////////////////////////////////////////////
bool ComportamientoTecnico::es_camino1(unsigned char c) const
{
  return (c == 'C' || c == 'D' || c == 'U' || c == 'S' || c == 'H');
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

Action ComportamientoTecnico::ComportamientoTecnicoNivel_0(Sensores sensores)
{
  Action accion;

  ActualizarMapa(sensores);

  // Si ya estamos en la planta de tratamiento, nos quedamos quietos
  if (sensores.superficie[0] == 'U') {
    return IDLE;
  }

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
 
//////////////////////////////////////////////////////////////////////////////////////////
//NIVEL E Y 2
//////////////////////////////////////////////////////////////////////////////////////////

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
Action ComportamientoTecnico::ComportamientoTecnicoNivel_2(Sensores sensores) { return IDLE; }
//////////////////////////////////////////////////////////////////////////////////////////
//NIVEL 3
//////////////////////////////////////////////////////////////////////////////////////////

int ComportamientoTecnico::Heuristica(const EstadoT &actual, const EstadoT &final) const{
  int dif_fila = std::abs(actual.site.f - final.site.f);
  int dif_col = std::abs(actual.site.c - final.site.c);
  return std::max(dif_fila, dif_col)  ;
}

bool ComportamientoTecnico::CasillaAccesibleTecnico(const EstadoT &st, const std::vector<std::vector<unsigned char>> &terreno, const std::vector<std::vector<unsigned char>> &altura) const {
    EstadoT next = NextCasillaTécnico(st);
  
    if (next.site.f < 0 || next.site.f >= terreno.size() || next.site.c < 0 || next.site.c >= terreno[0].size()) 
      return false;

    unsigned char dest = terreno[next.site.f][next.site.c];
    
    if(dest == 'P' || dest == 'M' || dest == '?')// ? para el nivel 6
      return false;

    if (dest == 'B' && !st.zapatillas) 
      return false;

    if (abs(altura[next.site.f][next.site.c] - altura[st.site.f][st.site.c]) > 1) 
      return false;

    if (casillas_bloqueadas.find({next.site.f, next.site.c}) != casillas_bloqueadas.end()) {
      return false;
    }

    return true;
}

int ComportamientoTecnico::CosteAccionTecnico(Action accion, const EstadoT &st, const std::vector<std::vector<unsigned char>> &terreno, 
  const std::vector<std::vector<unsigned char>> &altura) const{
      unsigned char terreno_actual = terreno[st.site.f][st.site.c];
      int coste = 0;

      switch (accion) {
        case WALK:{
          if (terreno_actual == 'A') coste = 60;
          else if (terreno_actual == 'H') coste = 6;
          else if (terreno_actual == 'S') coste = 3;
          else if (terreno_actual == 'B' && st.zapatillas) coste = 1; 
          else coste = 1; 

          EstadoT next = NextCasillaTécnico(st);
          int diff_altura = altura[next.site.f][next.site.c] - altura[st.site.f][st.site.c];
        
          if (diff_altura == 1) {
            coste += 5; 
          } else if (diff_altura == -1) {
            coste -= 2; 
          }

          if (coste < 1) coste = 1;
          break;
        }
        case TURN_SL: 
        case TURN_SR:
          if (terreno_actual == 'A') coste = 5;
          else if (terreno_actual == 'H') coste = 2;
          else if (terreno_actual == 'S') coste = 1;
          else if (terreno_actual == 'B' && st.zapatillas) coste = 1;
          else coste = 1; 
          break;
        default:
          coste = 0;
          break;
      }
      return coste;
}

std::list<Action> ComportamientoTecnico::A_Estrella(const EstadoT &inicio, const EstadoT &final, 
    const std::vector<std::vector<unsigned char>> &terreno, const std::vector<std::vector<unsigned char>> &altura){    
    
    std::priority_queue<NodoT> frontier;
    std::map<EstadoT, int> explored;

    NodoT nodo_inicial;
    nodo_inicial.estado = inicio; 
    nodo_inicial.secuencia = {};  
    nodo_inicial.g = 0; 
    nodo_inicial.h = Heuristica(inicio, final); 
    nodo_inicial.f = nodo_inicial.g + nodo_inicial.h;

    frontier.push(nodo_inicial);
    explored[inicio] = 0;

    while (!frontier.empty()) {
      NodoT current_node = frontier.top();
      frontier.pop();

      if (current_node.g > explored[current_node.estado]) {
          continue;
      }

      if (current_node.estado.site.f == final.site.f && current_node.estado.site.c == final.site.c) {
          return current_node.secuencia; 
      }
      
      Action acciones_posibles[] = {WALK, TURN_SL, TURN_SR};

      for(Action accion : acciones_posibles){
        if (accion == WALK && !CasillaAccesibleTecnico(current_node.estado, terreno, altura)) {
          continue; 
        }
        
        EstadoT estado_hijo = applyT(accion, current_node.estado, terreno, altura); 
        int coste_accion = CosteAccionTecnico(accion, current_node.estado, terreno, altura);
        int g_hijo = current_node.g + coste_accion;

        auto it_explored = explored.find(estado_hijo);
        
        if (it_explored == explored.end() || g_hijo < it_explored->second) {
          explored[estado_hijo] = g_hijo;

          NodoT nodo_hijo;
          nodo_hijo.estado = estado_hijo;
          nodo_hijo.secuencia = current_node.secuencia;
          nodo_hijo.secuencia.push_back(accion);
          nodo_hijo.g = g_hijo;
          nodo_hijo.h = Heuristica(estado_hijo, final);
          nodo_hijo.f = nodo_hijo.g + nodo_hijo.h;
        
          frontier.push(nodo_hijo);
        }
      }
    }
    return std::list<Action>(); 
}

/**
 * @brief Comportamiento del técnico para el Nivel 3.
 * @param sensores Datos actuales de los sensores.
 * @return Acción a realizar.
 */
Action ComportamientoTecnico::ComportamientoTecnicoNivel_3(Sensores sensores) {
  if (sensores.superficie[0] == 'D') 
    tiene_zapatillas = true;

  // si chocamos plan = inválido recalculamos.
  if (sensores.choque) {
    hayPlan = false;
  }

  // si no hay plan lo calculamos
  if (!hayPlan) {
    EstadoT inicio;
    inicio.site.f = sensores.posF;
    inicio.site.c = sensores.posC;
    inicio.site.brujula = sensores.rumbo;
    inicio.zapatillas = tiene_zapatillas;

    EstadoT final;
    final.site.f = sensores.BelPosF;
    final.site.c = sensores.BelPosC;

    plan = A_Estrella(inicio, final, mapaResultado, mapaCotas);
    hayPlan = !plan.empty();

    // ver plan 
    if (hayPlan) {
      VisualizaPlan(inicio.site, plan);
    }
  }

  // si hay plan ejecutar
  Action proxima_accion = IDLE;
  if (hayPlan && !plan.empty()) {
    proxima_accion = plan.front();
    plan.pop_front();
    
    // si era la ult acción, marcamos que ya no hay plan
    if (plan.empty()) {
      hayPlan = false;
    }
  }
  
  return proxima_accion;

}

//////////////////////////////////////////////////////////////////////////////////////////
//NIVEL 4
//////////////////////////////////////////////////////////////////////////////////////////
/**
 * @brief Comportamiento del técnico para el Nivel 4.
 * @param sensores Datos actuales de los sensores.
 * @return Acción a realizar.
 */
Action ComportamientoTecnico::ComportamientoTecnicoNivel_4(Sensores sensores) {
  return IDLE;
}

///////////////////////////////////////////////////////////////////////
//NIVEL 5 
/////////////////////////////////////////////////////////////////////// 
Action ComportamientoTecnico::OrientarseHacia(Orientacion actual, Orientacion objetivo) const{
  if(actual == objetivo)
    return IDLE;
  
  int dist = (objetivo - actual + 8 ) % 8;

  if(dist <= 4)
    return TURN_SR;
  return TURN_SL;
}

Orientacion ComportamientoTecnico::ObtenerOrientacionOrtogonal(const ubicacion &origen, const ubicacion &destino) const{
    if((destino.f - origen.f) == -1 && (destino.c - origen.c) == 0)
    return norte;
  else if((destino.f - origen.f) == 0 && (destino.c - origen.c) == 1)
    return este;
  else if((destino.f - origen.f) == 1 && (destino.c - origen.c) == 0)
    return sur;
  else
    return oeste;
}
ubicacion ComportamientoTecnico::ElegirPosicionParaTecnico(int fila_ing, int col_ing, int fila_sig, int col_sig, const std::vector<std::vector<unsigned char>> &terreno, const std::vector<std::vector<unsigned char>> &altura) const {
  ubicacion pos; pos.f = -1; pos.c = -1; pos.brujula = norte;
  ubicacion pos_fallback; pos_fallback.f = -1; 

  int pfila[4] = {-1, 0, 1, 0};
  int pcol[4] = {0, 1, 0, -1};

  for(int i = 0; i < 4; i++){
    int f_ady = fila_ing + pfila[i];
    int c_ady = col_ing + pcol[i];
    
    if(f_ady < 0 || c_ady < 0 || f_ady >= (int)terreno.size() || c_ady >= (int)terreno[0].size()) continue; 
    
    unsigned char sup = terreno[f_ady][c_ady];
    if(sup == 'M' || sup == 'P' || sup == 'A' || sup == '?') continue;
    
    int alt_ing = altura[fila_ing][col_ing];
    int alt_tec = altura[f_ady][c_ady];
    
    if (alt_ing > alt_tec || alt_ing < alt_tec - 1) continue;
    
    // si es casilla del sig tramo la guardamos como fallback pero no la priorizamos
    if(f_ady == fila_sig && c_ady == col_sig) {
        pos_fallback.f = f_ady;
        pos_fallback.c = c_ady;
        continue;
    }
    
    pos.f = f_ady; pos.c = c_ady;
    return pos;
  }
  
  if (pos_fallback.f != -1) {
      pos.f = pos_fallback.f; pos.c = pos_fallback.c;
  }
  return pos;
}

bool ComportamientoTecnico::TramoTuberiaValido(const EstadoTuberia &actual, int sig_fila, int sig_col, int sig_op, const std::vector<std::vector<unsigned char>> &terreno, const std::vector<std::vector<unsigned char>> &altura) const
{
  if (sig_fila < 0 || sig_fila >= terreno.size() || sig_col < 0 || sig_col >= terreno[0].size())
    return false;
    
  unsigned char sup = terreno[sig_fila][sig_col];

  if (sup == 'P' || sup == 'M' || sup == '?')   // ? para el nivel 6
    return false;
    
  int altura_tuberia_actual = altura[actual.fila][actual.columna] + actual.op;
  int altura_tuberia_siguiente = altura[sig_fila][sig_col] + sig_op;
  
  if (altura_tuberia_siguiente != altura_tuberia_actual && altura_tuberia_siguiente != (altura_tuberia_actual - 1))
    return false;
    
  if (sup == 'A' && sig_op != 0)
    return false;
    
  return true;
}


std::list<Paso> ComportamientoTecnico::PlanificarRedTuberias(int inicioF, int inicioC, const std::vector<std::vector<unsigned char>> &terreno, const std::vector<std::vector<unsigned char>> &altura)
{
  priority_queue<NodoTuberia> frontier;
  map<EstadoTuberia, int> explored; // guarda costo min con el que se llego
  int operaciones[3] = {-1, 0, 1};

  // casilla orig
  for (int j = 0; j < 3; j++)
  {
    int o = operaciones[j];

    // si es agua, solo op 0
    if (terreno[inicioF][inicioC] == 'A' && o != 0)
      continue;

    EstadoTuberia e_ini = {inicioF, inicioC, o};
    NodoTuberia n_ini;
    n_ini.estado = e_ini;
    n_ini.secuencia.push_back({inicioF, inicioC, o});

    // calculo impacto
    int costo = 0;
    unsigned char sup = terreno[inicioF][inicioC];
    if (sup == 'A') costo = 50;
    else if (sup == 'H') costo = 45;
    else if (sup == 'S') costo = 25;
    else if (sup == 'C' || sup == 'U' || sup == 'D' || sup == 'X') costo = 15;
    else costo = 30; 

    if (o == 1) { // RAISE
      if (sup == 'H') costo += 55;
      else if (sup == 'S') costo += 30;
      else if (sup == 'C' || sup == 'U' || sup == 'D' || sup == 'X') costo += 10;
      else costo += 40;
    } else if (o == -1) { // DIG
      if (sup == 'H') costo += 65;
      else if (sup == 'S') costo += 40;
      else if (sup == 'C' || sup == 'U' || sup == 'D' || sup == 'X') costo += 25;
      else costo += 50;
    }
    
    n_ini.costo = costo;

    frontier.push(n_ini);
    explored[e_ini] = costo;
  }

  int posF[4] = {0, -1, 1, 0};
  int posC[4] = {-1, 0, 0, 1};

  while (!frontier.empty())
  {
    NodoTuberia nodoActual = frontier.top();
    frontier.pop();

    EstadoTuberia actual = nodoActual.estado;

    if (terreno[actual.fila][actual.columna] == 'U')
    {
      return nodoActual.secuencia;
    }

    // si habiamos encontrado 1 camino más barato a este estado lo ignoramos
    if (explored[actual] < nodoActual.costo) continue;

    // generamos hijos en las 4 direcciones
    for (int dir = 0; dir < 4; dir++)
    {
      int sig_fila = actual.fila + posF[dir];
      int sig_col = actual.columna + posC[dir];

      for (int op_idx = 0; op_idx < 3; op_idx++)
      {
        int sig_op = operaciones[op_idx];

        if (TramoTuberiaValido(actual, sig_fila, sig_col, sig_op, terreno, altura))
        {
          EstadoTuberia estado_hijo = {sig_fila, sig_col, sig_op};

          // calcular impacto tramo
          int costo_tramo = 0;
          unsigned char sup = terreno[sig_fila][sig_col];
          if (sup == 'A') costo_tramo = 50;
          else if (sup == 'H') costo_tramo = 45;
          else if (sup == 'S') costo_tramo = 25;
          else if (sup == 'C' || sup == 'U' || sup == 'D' || sup == 'X') costo_tramo = 15;
          else costo_tramo = 30; 

          if (sig_op == 1) { 
            if (sup == 'H') costo_tramo += 55;
            else if (sup == 'S') costo_tramo += 30;
            else if (sup == 'C' || sup == 'U' || sup == 'D' || sup == 'X') costo_tramo += 10;
            else costo_tramo += 40;
          } else if (sig_op == -1) { 
            if (sup == 'H') costo_tramo += 65;
            else if (sup == 'S') costo_tramo += 40;
            else if (sup == 'C' || sup == 'U' || sup == 'D' || sup == 'X') costo_tramo += 25;
            else costo_tramo += 50;
          }

          // +1 sirve como desempatador para preferir caminos más cortos si el impacto es el mismo
          int nuevo_costo = nodoActual.costo + costo_tramo + 1;

          // si nunca hemos visitado el nodo o si hemos encontrado una ruta de < impacto
          if (explored.find(estado_hijo) == explored.end() || nuevo_costo < explored[estado_hijo])
          {
            explored[estado_hijo] = nuevo_costo;
            
            NodoTuberia hijo;
            hijo.estado = estado_hijo;
            hijo.secuencia = nodoActual.secuencia;
            hijo.secuencia.push_back({sig_fila, sig_col, sig_op});
            hijo.costo = nuevo_costo;

            frontier.push(hijo);
          }
        }
      }
    }
  }

  // si no hay camino
  return std::list<Paso>();
}

Action ComportamientoTecnico::ComportamientoTecnicoNivel_5(Sensores sensores)
{
  if (sensores.superficie[0] == 'D') tiene_zapatillas = true;

  if (sensores.choque) {
    plan.clear();
    hayPlan = false; 
    ubicacion frente = Delante({sensores.posF, sensores.posC, sensores.rumbo});
    casillas_bloqueadas.insert({frente.f, frente.c}); 
  }

  // calculo para saber sig casilla
    if (!red_planificada) {
    planTuberias = PlanificarRedTuberias(sensores.BelPosF, sensores.BelPosC, mapaResultado, mapaCotas);
    red_planificada = true; 

    if (planTuberias.empty()) {
      red_completada = true;
    }
    
    // iniciar su var de tramo anterior
    tramo_ant_f = -1;
    tramo_ant_c = -1;
  }

  // llamada ing
   if (sensores.venpaca) {
    if (ing_f_actual != sensores.GotoF || ing_c_actual != sensores.GotoC) {
      ing_f_actual = sensores.GotoF;
      ing_c_actual = sensores.GotoC;

      if (!planTuberias.empty() && planTuberias.front().fil == ing_f_actual && planTuberias.front().col == ing_c_actual) {
        planTuberias.pop_front();
      }

      if (tramo_ant_f != -1) {
        pos_objetivo_actual.f = tramo_ant_f;
        pos_objetivo_actual.c = tramo_ant_c;
      } else {
        int f_sig = planTuberias.empty() ? -1 : planTuberias.front().fil;
        int c_sig = planTuberias.empty() ? -1 : planTuberias.front().col;
        pos_objetivo_actual = ElegirPosicionParaTecnico(ing_f_actual, ing_c_actual, f_sig, c_sig, mapaResultado, mapaCotas);
      }
      
      if (pos_objetivo_actual.f != -1) {
        estado_asistencia = 1;
        plan.clear();
        hayPlan = false;
      }
    }
  }
  
 if (estado_asistencia == 0) { 
    return IDLE;
  }

  if (estado_asistencia == 1) { // ciajar a la pos 
    if (sensores.posF == pos_objetivo_actual.f && sensores.posC == pos_objetivo_actual.c) {
      estado_asistencia = 2;
      casillas_bloqueadas.clear();
    } else {
      if (!hayPlan) {
        EstadoT inicio = { {sensores.posF, sensores.posC, sensores.rumbo}, tiene_zapatillas};
        EstadoT final = { {pos_objetivo_actual.f, pos_objetivo_actual.c, norte}, false};
        plan = A_Estrella(inicio, final, mapaResultado, mapaCotas);
        hayPlan = true;
      }
      
      if (!plan.empty()) {
        Action act = plan.front(); 
        plan.pop_front();
        return act;
      }
      return IDLE; 
    }
  }

  if (estado_asistencia == 2) { // giar y sincronizar install
    ubicacion mi_pos = {sensores.posF, sensores.posC, sensores.rumbo};
    ubicacion pos_ing = {ing_f_actual, ing_c_actual, norte}; 
    Orientacion ideal = ObtenerOrientacionOrtogonal(mi_pos, pos_ing);
    
    if (sensores.rumbo != ideal) {
      return OrientarseHacia(sensores.rumbo, ideal);
    }
    
    if (sensores.enfrente) {
      estado_asistencia = 0;
      
      tramo_ant_f = ing_f_actual;
      tramo_ant_c = ing_c_actual;
      
      return INSTALL;        
    }
    return IDLE; 
  }

  return IDLE;
}

///////////////////////////////////////////////////////////////////////
//NIVEL 6 
/////////////////////////////////////////////////////////////////////// 

/**
 * @brief Comportamiento del técnico para el Nivel 6.
 * @param sensores Datos actuales de los sensores.
 * @return Acción a realizar.
 */
Action ComportamientoTecnico::ComportamientoTecnicoNivel_6(Sensores sensores) 
{
  ActualizarMapa(sensores);
  if (sensores.superficie[0] == 'D') tiene_zapatillas = true;

  // orden ingeniero
  if (sensores.venpaca) {
    ing_f_actual = sensores.GotoF;
    ing_c_actual = sensores.GotoC;

    if (tramo_ant_f != -1) {
      pos_objetivo_actual.f = tramo_ant_f;
      pos_objetivo_actual.c = tramo_ant_c;
    } else {
      pos_objetivo_actual = ElegirPosicionParaTecnico(ing_f_actual, ing_c_actual, -1, -1, mapaResultado, mapaCotas);
    }
    
    // si no conocemos la zona ? vamos hacia el ingeniero
    if (pos_objetivo_actual.f == -1) {
      pos_objetivo_actual.f = ing_f_actual;
      pos_objetivo_actual.c = ing_c_actual;
    }
    
    estado_asistencia = 1;
    plan.clear();
    hayPlan = false;
  }


  // estado 0: EXPLORADOR 
  if (estado_asistencia == 0) { 
    // Solo explora si no ha participado en la tuberia
    if (tramo_ant_f == -1) {
      return ComportamientoTecnicoNivel_1(sensores);
    }
    // si ya es parte se queda quito
    return IDLE; 
  }

  // colisones
  if (sensores.choque && estado_asistencia == 1) {
    if (sensores.agentes[2] == 'i') {
      // si nos chocamos con el ingeniero, esperamos.
      if (last_action != IDLE) plan.push_front(last_action);
      return IDLE;
    } else {
      plan.clear(); 
      hayPlan = false; 
      ubicacion frente = Delante({sensores.posF, sensores.posC, sensores.rumbo});
      casillas_bloqueadas.insert({frente.f, frente.c}); 
    }
  }

  // estado 1 IR HACIA EL INGENIERO 
  if (estado_asistencia == 1) { 
    // Calculamos la distancia Manhattan
    int dist = abs(sensores.posF - pos_objetivo_actual.f) + abs(sensores.posC - pos_objetivo_actual.c);
    
    // comprobar si hemos llegado a la casilla exacta 
    bool objetivo_alcanzado = (sensores.posF == pos_objetivo_actual.f && sensores.posC == pos_objetivo_actual.c);
    bool cerca_del_ingeniero = (dist <= 1 && pos_objetivo_actual.f == ing_f_actual && pos_objetivo_actual.c == ing_c_actual);

    if (objetivo_alcanzado || cerca_del_ingeniero) {
      estado_asistencia = 2;
      casillas_bloqueadas.clear();
    } else {
      if (!hayPlan) {
        EstadoT inicio = { {sensores.posF, sensores.posC, sensores.rumbo}, tiene_zapatillas};
        EstadoT final = { {pos_objetivo_actual.f, pos_objetivo_actual.c, norte}, false};
        plan = A_Estrella(inicio, final, mapaResultado, mapaCotas);
        
        if (plan.empty()) {
          return TURN_SR; 
        } else {
          hayPlan = true;
        }
      }
      
      if (!plan.empty()) {
        Action act = plan.front(); 
        plan.pop_front(); 
        last_action = act;
        return act;
      }
    }
  }

  // estado 2: GIRAR Y SINCRONIZAR INSTALL 
  if (estado_asistencia == 2) { 
    ubicacion mi_pos = {sensores.posF, sensores.posC, sensores.rumbo};
    ubicacion pos_ing = {ing_f_actual, ing_c_actual, norte}; 
    Orientacion ideal = ObtenerOrientacionOrtogonal(mi_pos, pos_ing);
    
    if (sensores.rumbo != ideal) {
      last_action = OrientarseHacia(sensores.rumbo, ideal);
      return last_action;
    }
    
    // si estamos mirando al ingeniero y él nos mira a nosotros 
    if (sensores.enfrente) {
      estado_asistencia = 0; // vuelve a estado quieto.
      tramo_ant_f = ing_f_actual;
      tramo_ant_c = ing_c_actual;
      last_action = INSTALL;
      return INSTALL;        
    }
    // si el ingeniero aún no está mirando esperamos 
    return IDLE; 
  }

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

