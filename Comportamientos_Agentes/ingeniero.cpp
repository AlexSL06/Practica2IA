#include "ingeniero.hpp"
#include "motorlib/util.h"
#include <iostream>
#include <queue>


using namespace std;

// =========================================================================
// ÁREA DE IMPLEMENTACIÓN DEL ESTUDIANTE
// =========================================================================

Action ComportamientoIngeniero::think(Sensores sensores)
{
  Action accion = IDLE;

  switch (sensores.nivel)
  {
  /*case 0: accion = ComportamientoIngenieroNivel_0(sensores); break;
  case 1: accion = ComportamientoIngenieroNivel_1(sensores); break;*/
  case 2: accion = ComportamientoIngenieroNivel_2(sensores); break;
  case 3: accion = ComportamientoIngenieroNivel_3(sensores); break;
  case 4: accion = ComportamientoIngenieroNivel_4(sensores); break;
  case 5: accion = ComportamientoIngenieroNivel_5(sensores); break;
  case 6: accion = ComportamientoIngenieroNivel_6(sensores); break;
  }

  return accion;
}

int ComportamientoIngeniero::veoCasillaInteresanteI0(char i, char c, char d, bool zaps)
{
  if (c == 'U') 
    return 2;
  else if (d == 'U') 
    return 3;
  else if (i == 'U') 
    return 1;
  else if (!zaps) {
    if (c == 'D') 
      return 2;
    else if (d == 'D') 
      return 3;
    else if (i == 'D') 
      return 1;
  }

  if (c == 'C' || c == 'D') 
    return 2;
  else if (d == 'C' || d == 'D') 
    return 3;
  else if (i == 'C' || i == 'D') 
    return 1;

  return 0;
}

int ComportamientoIngeniero::veoCasillaInteresanteI1(char i, char c, char d, bool zaps)
{
  if (!zaps) {
    if (c == 'D') 
      return 2;
    else if (d == 'D') 
      return 3;
    else if (i == 'D') 
      return 1;
  }

  if (es_camino(c)) 
    return 2;
  else if (es_camino(d)) 
    return 3;
  else if (es_camino(i)) 
    return 1;
  else if (c == 'S') 
    return 2;
  else if (d == 'S') 
    return 3;
  else if (i == 'S') 
    return 1;

  return 0;
}

char ComportamientoIngeniero::viablePorAlturaI(char casilla, int dif, bool zap)
{
  if (abs(dif) <= 1 || (abs(dif) <= 2 && zap)) 
    return casilla;
  else 
    return 'P';
}

bool ComportamientoIngeniero::puedeSaltarI(const Sensores &sensores, bool tiene_zaps)
{
  if (sensores.agentes[2] != '_' || sensores.superficie[2] == 'M' || sensores.superficie[2] == 'P' || sensores.superficie[2] == 'B') {
    return false;
  }

  if (!es_camino(sensores.superficie[6])) {
    return false;
  }

  int diff_altura = abs(sensores.cota[6] - sensores.cota[0]);
  int limite = tiene_zaps ? 3 : 2;
  if (diff_altura >= limite) return false;

  return true;
}

Action ComportamientoIngeniero::ComportamientoIngenieroNivel_0(Sensores sensores)
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

  if ((last_action == WALK) &&
      sensores.posF == last_f && sensores.posC == last_c) {
    en_bloqueo = true;
    last_action = giro_preferido;
    return giro_preferido;
  }

  if ((last_action == JUMP) &&
      sensores.posF == last_f && sensores.posC == last_c) {
    en_bloqueo_J = true;
    last_action = giro_preferido;
    return giro_preferido;
  }

  char i = viablePorAlturaI(sensores.superficie[1], sensores.cota[1] - sensores.cota[0], tiene_zapatillas);
  char c = viablePorAlturaI(sensores.superficie[2], sensores.cota[2] - sensores.cota[0], tiene_zapatillas);
  char d = viablePorAlturaI(sensores.superficie[3], sensores.cota[3] - sensores.cota[0], tiene_zapatillas);

  int pos = veoCasillaInteresanteI0(i, c, d, tiene_zapatillas);

  if (sensores.superficie[0] == 'U') {
    if (c == 'U') return WALK;
    else if (d == 'U') return TURN_SR;
    else if (i == 'U') return TURN_SL;
    else return IDLE;
  }

  if ((en_bloqueo && c == 'U') || (en_bloqueo_J && sensores.superficie[6] == 'U')) {
    en_bloqueo_U = true;
    contador_giros++;
    last_action = giro_preferido;
    return giro_preferido;
  }

  if (en_bloqueo_U) {
    if (pos == 2 && en_bloqueo) {
      en_bloqueo_U = false;
      en_bloqueo = false;
      accion = WALK;
    }
    else if (en_bloqueo_J && puedeSaltarI(sensores, tiene_zapatillas)) {
      en_bloqueo_U = false;
      en_bloqueo = false;
      accion = JUMP;
    }
    else {
      contador_giros++;
      accion = giro_preferido;
    }
    last_action = accion;
    return accion;
  }
  else if (en_bloqueo) {
    if (pos == 2) {
      en_bloqueo = false;
      accion = WALK;
    }
    else accion = giro_preferido;
    last_action = accion;
    return accion;
  }
  else if (en_bloqueo_J) {
    if (pos == 2) {
      en_bloqueo_J = false;
      accion = WALK;
    }
    else if (puedeSaltarI(sensores, tiene_zapatillas)) {
      en_bloqueo_J = false;
      accion = JUMP;
    }
    else accion = giro_preferido;
    last_action = accion;
    return accion;
  }

  if (en_bloqueo && sensores.superficie[6] == 'U' && puedeSaltarI(sensores, tiene_zapatillas)) {
    last_action = JUMP;
    return JUMP;
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
    else if (puedeSaltarI(sensores, tiene_zapatillas) && (pos == 0)) {
      accion = JUMP;
      giro_defecto = false;
    }
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

bool ComportamientoIngeniero::es_camino(unsigned char c) const
{
  return (c == 'C' || c == 'D' || c == 'U');
}

bool ComportamientoIngeniero::es_camino1(unsigned char c) const
{
  return (c == 'C' || c == 'D' || c == 'U' || c == 'S');
}

Action ComportamientoIngeniero::ComportamientoIngenieroNivel_1(Sensores sensores)
{
  Action accion;

  ActualizarMapa(sensores);

  if (sensores.superficie[0] == 'D')  tiene_zapatillas = true;

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

  if ((last_action == WALK) &&
      sensores.posF == last_f && sensores.posC == last_c) {
    en_bloqueo = true;
    last_action = giro_preferido;
    return giro_preferido;
  }

  if ((last_action == JUMP) &&
      sensores.posF == last_f && sensores.posC == last_c) {
    en_bloqueo_J = true;
    last_action = giro_preferido;
    return giro_preferido;
  }

  char i = viablePorAlturaI(sensores.superficie[1], sensores.cota[1] - sensores.cota[0], tiene_zapatillas);
  char c = viablePorAlturaI(sensores.superficie[2], sensores.cota[2] - sensores.cota[0], tiene_zapatillas);
  char d = viablePorAlturaI(sensores.superficie[3], sensores.cota[3] - sensores.cota[0], tiene_zapatillas);

  int pos = veoCasillaInteresanteI1(i, c, d, tiene_zapatillas);

  if (en_bloqueo) {
    if (es_camino1(sensores.superficie[2]) && sensores.superficie[2] == c) {
      en_bloqueo = false;
      accion = WALK;
    }
    else accion = giro_preferido;
    last_action = accion;
    return accion;
  }
  else if (en_bloqueo_J) {
    if (es_camino1(sensores.superficie[2]) && sensores.superficie[2] == c) {
      en_bloqueo_J = false;
      accion = WALK;
    }
    else if (puedeSaltarI(sensores, tiene_zapatillas)) {
      en_bloqueo_J = false;
      accion = JUMP;
    }
    else accion = giro_preferido;
    last_action = accion;
    return accion;
  }

  if (es_camino1(sensores.superficie[1]) && (i != 'P') &&
      visitas[pos_izq.f][pos_izq.c] < visitas[pos_frente.f][pos_frente.c]) {
    accion = TURN_SL;
  }
  else if (es_camino1(sensores.superficie[3]) && (d != 'P') &&
           visitas[pos_der.f][pos_der.c] < visitas[pos_frente.f][pos_frente.c]) {
    accion = TURN_SR;
  }
  else if (es_camino1(sensores.superficie[2]) && (c != 'P') && sensores.superficie[2] == c) {
    accion = WALK;
  }
  else if (pos == 2) { accion = WALK; giro_defecto = false; }
  else if (pos == 3) { accion = TURN_SR; giro_defecto = false; }
  else if (pos == 1) { accion = TURN_SL; giro_defecto = false; }
  else if (puedeSaltarI(sensores, tiene_zapatillas) && (pos == 0)) {
    accion = JUMP;
    giro_defecto = false;
  }
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

EstadoI ComportamientoIngeniero::NextCasillaIngeniero(const EstadoI &st) const {
  EstadoI siguiente = st;
  switch (st.orientacion) {
    case norte:    siguiente.fila = st.fila - 1; break;
    case noreste:  siguiente.fila = st.fila - 1; siguiente.columna = st.columna + 1; break;
    case este:     siguiente.columna = st.columna + 1; break;
    case sureste:  siguiente.fila = st.fila + 1; siguiente.columna = st.columna + 1; break;
    case sur:      siguiente.fila = st.fila + 1; break;
    case suroeste: siguiente.fila = st.fila + 1; siguiente.columna = st.columna - 1; break;
    case oeste:    siguiente.columna = st.columna - 1; break;
    case noroeste: siguiente.fila = st.fila - 1; siguiente.columna = st.columna - 1; break;
  }
  return siguiente;
}
bool ComportamientoIngeniero::CasillaAccesibleIngeniero(const EstadoI &st, const vector<vector<unsigned char>> &terreno, const vector<vector<unsigned char>> &altura) const {
  EstadoI next = NextCasillaIngeniero(st);
  
  // Fuera de límites
  if (next.fila < 0 || next.fila >= terreno.size() || next.columna < 0 || next.columna >= terreno[0].size()) return false;
  
  unsigned char sup = terreno[next.fila][next.columna];
  // ¡IMPORTANTE! El bosque ('B') SIEMPRE es intransitable para el Ingeniero (PDF Pág. 6)
  if (sup == 'P' || sup == 'M' || sup == 'B') {
    return false;
  }

  int diff = altura[next.fila][next.columna] - altura[st.fila][st.columna];
  int limite_altura = st.zapatillas ? 2 : 1;
  
  return abs(diff) <= limite_altura;
}

bool ComportamientoIngeniero::CasillaAccesibleJumpIngeniero(const EstadoI &st, const vector<vector<unsigned char>> &terreno, const vector<vector<unsigned char>> &altura) const {
  EstadoI inter = NextCasillaIngeniero(st);
  
  // Casilla intermedia
  if (inter.fila < 0 || inter.fila >= terreno.size() || inter.columna < 0 || inter.columna >= terreno[0].size()) return false;
  unsigned char sup_inter = terreno[inter.fila][inter.columna];
  if (sup_inter == 'P' || sup_inter == 'M' || sup_inter == 'B') return false;

  // REGLA FÍSICA: Al saltar, no podemos atravesar una pared más alta que nuestra posición actual
  if (altura[inter.fila][inter.columna] > altura[st.fila][st.columna]) return false;

  // Casilla final de aterrizaje
  EstadoI final_st = NextCasillaIngeniero(inter);
  if (final_st.fila < 0 || final_st.fila >= terreno.size() || final_st.columna < 0 || final_st.columna >= terreno[0].size()) return false;
  unsigned char sup_final = terreno[final_st.fila][final_st.columna];
  if (sup_final == 'P' || sup_final == 'M' || sup_final == 'B') return false;

  int diff = altura[final_st.fila][final_st.columna] - altura[st.fila][st.columna];
  int limite_altura = st.zapatillas ? 2 : 1;
  
  return abs(diff) <= limite_altura;
}

EstadoI ComportamientoIngeniero::applyI(Action accion, const EstadoI &st, const vector<vector<unsigned char>> &terreno, const vector<vector<unsigned char>> &altura) const {
  EstadoI next = st;
  switch (accion) {
    case WALK:
      next = NextCasillaIngeniero(st);
      if (terreno[next.fila][next.columna] == 'D') next.zapatillas = true;
      break;
    case JUMP:
      next = NextCasillaIngeniero(NextCasillaIngeniero(st));
      if (terreno[next.fila][next.columna] == 'D') next.zapatillas = true;
      break;
    case TURN_SR:
      next.orientacion = (Orientacion)((next.orientacion + 1) % 8);
      break;
    case TURN_SL:
      next.orientacion = (Orientacion)((next.orientacion + 7) % 8);
      break;
    default:
      break;
  }
  return next;
}
list<Action> ComportamientoIngeniero::B_Anchura(const EstadoI &inicio, const EstadoI &final, const vector<vector<unsigned char>> &terreno, const vector<vector<unsigned char>> &altura) {
  queue<NodoI> frontier;
  set<EstadoI> explored; 
  list<Action> path;

  // Caso borde: Si ya estamos en la meta al empezar
  if (inicio.fila == final.fila && inicio.columna == final.columna) {
    return path; 
  }

  NodoI current_node;
  current_node.estado = inicio;
  
  frontier.push(current_node);
  explored.insert(current_node.estado);

  while (!frontier.empty()) {
    current_node = frontier.front();
    frontier.pop();

    // Priorizamos JUMP porque avanza 2 casillas en 1 solo instante de simulación
    Action acciones[] = {JUMP, WALK, TURN_SL, TURN_SR};
    
    for (Action accion : acciones) {
      bool posible = false;
      
      // Comprobamos la legalidad de la acción
      if (accion == WALK) {
        posible = CasillaAccesibleIngeniero(current_node.estado, terreno, altura);
      } else if (accion == JUMP) {
        posible = CasillaAccesibleJumpIngeniero(current_node.estado, terreno, altura);
      } else {
        posible = true; // Girar siempre es legal
      }

      if (posible) {
        NodoI child = current_node;
        child.estado = applyI(accion, current_node.estado, terreno, altura);
        child.secuencia.push_back(accion);
        
        // EARLY GOAL TEST: Verificamos la meta antes de meterlo en la cola
        if (child.estado.fila == final.fila && child.estado.columna == final.columna) {
          return child.secuencia; // Hemos encontrado la ruta más corta
        }

        // Si es un estado no explorado, lo añadimos
        if (explored.find(child.estado) == explored.end()) {
          explored.insert(child.estado);
          frontier.push(child);
        }
      }
    }
  }

  return path; // Si no hay camino, devuelve lista vacía
}

Action ComportamientoIngeniero::ComportamientoIngenieroNivel_2(Sensores sensores) {
  // 1. GESTIÓN DE COLISIONES (¡NUEVO!)
  // Si nos hemos chocado en el instante anterior, la acción falló.
  if (sensores.choque) {
    // Si chocamos porque el Técnico ('t') está justo delante, simplemente le damos tiempo a apartarse.
    if (sensores.agentes[2] == 't') {
      // Re-insertamos la acción que falló al principio del plan para intentarlo de nuevo luego
      plan.push_front(last_action);
      return IDLE; // Esperamos un turno sin hacer nada
    } else {
      // Si chocamos contra un árbol o muro (el plan era defectuoso), borramos el plan para recalcular
      hayPlan = false;
      plan.clear();
    }
  }

  // 2. CÁLCULO DEL PLAN (Si no tenemos uno)
  if (!hayPlan) {
    EstadoI inicio;
    inicio.fila = sensores.posF;
    inicio.columna = sensores.posC;
    inicio.orientacion = sensores.rumbo;
    
    // Verificamos si empezamos sobre unas zapatillas
    if (sensores.superficie[0] == 'D') tiene_zapatillas = true;
    inicio.zapatillas = tiene_zapatillas;

    EstadoI meta;
    meta.fila = sensores.BelPosF;
    meta.columna = sensores.BelPosC;

    // Llamamos a nuestro planificador BFS
    plan = B_Anchura(inicio, meta, mapaResultado, mapaCotas);
    hayPlan = (plan.size() > 0);
    
    if (hayPlan) {
      ubicacion st_ini = {inicio.fila, inicio.columna, inicio.orientacion};
      VisualizaPlan(st_ini, plan);
    }
  }
  
  // 3. EJECUCIÓN DEL PLAN
  if (hayPlan && plan.size() > 0) {
    Action accion_a_realizar = plan.front();
    plan.pop_front(); // Quitamos la acción de la lista
    
    if (plan.size() == 0) {
      hayPlan = false;
    }
    
    last_action = accion_a_realizar; // Guardamos la acción por si chocamos en el siguiente turno
    return accion_a_realizar;
  }
  
  // Si no hay solución posible
  return IDLE;
}

Action ComportamientoIngeniero::ComportamientoIngenieroNivel_3(Sensores sensores) { return IDLE; }
Action ComportamientoIngeniero::ComportamientoIngenieroNivel_4(Sensores sensores) { return IDLE; }
Action ComportamientoIngeniero::ComportamientoIngenieroNivel_5(Sensores sensores) { return IDLE; }
Action ComportamientoIngeniero::ComportamientoIngenieroNivel_6(Sensores sensores) { return IDLE; }

// =========================================================================
// FUNCIONES PROPORCIONADAS
// =========================================================================

void ComportamientoIngeniero::ActualizarMapa(Sensores sensores)
{
  mapaResultado[sensores.posF][sensores.posC] = sensores.superficie[0];
  mapaCotas[sensores.posF][sensores.posC] = sensores.cota[0];

  int pos = 1;
  switch (sensores.rumbo)
  {
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
/*
bool ComportamientoIngeniero::EsCasillaTransitableLevel0(int f, int c, bool tieneZapatillas)
{
  if (f < 0 || f >= (int)mapaResultado.size() || c < 0 || c >= (int)mapaResultado[0].size())
    return false;
  return es_camino(mapaResultado[f][c]);
}
*/
bool ComportamientoIngeniero::EsAccesiblePorAltura(const ubicacion &actual, bool zap)
{
  ubicacion del = Delante(actual);
  if (del.f < 0 || del.f >= (int)mapaCotas.size() || del.c < 0 || del.c >= (int)mapaCotas[0].size())
    return false;
  int desnivel = abs(mapaCotas[del.f][del.c] - mapaCotas[actual.f][actual.c]);
  if (zap && desnivel > 2) return false;
  if (!zap && desnivel > 1) return false;
  return true;
}

ubicacion ComportamientoIngeniero::Delante(const ubicacion &actual) const
{
  ubicacion delante = actual;
  switch (actual.brujula)
  {
  case 0: delante.f--; break;
  case 1: delante.f--; delante.c++; break;
  case 2: delante.c++; break;
  case 3: delante.f++; delante.c++; break;
  case 4: delante.f++; break;
  case 5: delante.f++; delante.c--; break;
  case 6: delante.c--; break;
  case 7: delante.f--; delante.c--; break;
  }
  return delante;
}

void ComportamientoIngeniero::PintaPlan(const list<Action> &plan)
{
  auto it = plan.begin();
  while (it != plan.end()) {
    if (*it == WALK) cout << "W ";
    else if (*it == JUMP) cout << "J ";
    else if (*it == TURN_SR) cout << "r ";
    else if (*it == TURN_SL) cout << "l ";
    else if (*it == COME) cout << "C ";
    else if (*it == IDLE) cout << "I ";
    else cout << "-_ ";
    it++;
  }
  cout << "( longitud " << plan.size() << ")" << endl;
}

void ComportamientoIngeniero::PintaPlan(const list<Paso> &plan)
{
  auto it = plan.begin();
  while (it != plan.end()) {
    cout << it->fil << ", " << it->col << " (" << it->op << ")\n";
    it++;
  }
  cout << "( longitud " << plan.size() << ")" << endl;
}

void ComportamientoIngeniero::VisualizaPlan(const ubicacion &st, const list<Action> &plan)
{
  listaPlanCasillas.clear();
  ubicacion cst = st;

  listaPlanCasillas.push_back({cst.f, cst.c, WALK});
  auto it = plan.begin();
  while (it != plan.end()) {
    switch (*it) {
    case JUMP:
      switch (cst.brujula) {
      case 0: cst.f--; break;
      case 1: cst.f--; cst.c++; break;
      case 2: cst.c++; break;
      case 3: cst.f++; cst.c++; break;
      case 4: cst.f++; break;
      case 5: cst.f++; cst.c--; break;
      case 6: cst.c--; break;
      case 7: cst.f--; cst.c--; break;
      }
      if (cst.f >= 0 && cst.f < (int)mapaResultado.size() &&
          cst.c >= 0 && cst.c < (int)mapaResultado[0].size())
        listaPlanCasillas.push_back({cst.f, cst.c, JUMP});
    case WALK:
      switch (cst.brujula) {
      case 0: cst.f--; break;
      case 1: cst.f--; cst.c++; break;
      case 2: cst.c++; break;
      case 3: cst.f++; cst.c++; break;
      case 4: cst.f++; break;
      case 5: cst.f++; cst.c--; break;
      case 6: cst.c--; break;
      case 7: cst.f--; cst.c--; break;
      }
      if (cst.f >= 0 && cst.f < (int)mapaResultado.size() &&
          cst.c >= 0 && cst.c < (int)mapaResultado[0].size())
        listaPlanCasillas.push_back({cst.f, cst.c, WALK});
      break;
    case TURN_SR:
      cst.brujula = (Orientacion)(((int)cst.brujula + 1) % 8);
      break;
    case TURN_SL:
      cst.brujula = (Orientacion)(((int)cst.brujula + 7) % 8);
      break;
    default: break;
    }
    it++;
  }
}

void ComportamientoIngeniero::VisualizaRedTuberias(const list<Paso> &plan)
{
  listaCanalizacionTuberias.clear();
  auto it = plan.begin();
  while (it != plan.end()) {
    listaCanalizacionTuberias.push_back({it->fil, it->col, it->op});
    it++;
  }
}
