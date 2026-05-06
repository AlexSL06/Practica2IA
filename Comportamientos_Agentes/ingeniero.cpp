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
  case 0:
    accion = ComportamientoIngenieroNivel_0(sensores);
    break;
  case 1:
    accion = ComportamientoIngenieroNivel_1(sensores);
    break;
  case 2:
    accion = ComportamientoIngenieroNivel_2(sensores);
    break;
  case 3:
    accion = ComportamientoIngenieroNivel_3(sensores);
    break;
  case 4:
    accion = ComportamientoIngenieroNivel_4(sensores);
    break;
  case 5:
    accion = ComportamientoIngenieroNivel_5(sensores);
    break;
  case 6:
    accion = ComportamientoIngenieroNivel_6(sensores);
    break;
  }

  return accion;
}

/////////////////////////////////////////////////////////
//NIVEL 0
/////////////////////////////////////////////////////////
int ComportamientoIngeniero::veoCasillaInteresanteI0(char i, char c, char d, bool zaps)
{
  if (c == 'U')
    return 2;
  else if (d == 'U')
    return 3;
  else if (i == 'U')
    return 1;
  else if (!zaps)
  {
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
bool ComportamientoIngeniero::es_camino(unsigned char c) const{
  return (c == 'C' || c == 'D' || c == 'U');
}

/////////////////////////////////////////////////////////
//NIVEL 1
/////////////////////////////////////////////////////////
int ComportamientoIngeniero::veoCasillaInteresanteI1(char i, char c, char d, bool zaps)
{
  if (!zaps)
  {
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
bool ComportamientoIngeniero::es_camino1(unsigned char c) const
{
  return (c == 'C' || c == 'D' || c == 'U' || c == 'S');
}

bool ComportamientoIngeniero::puedeSaltarI(const Sensores &sensores, bool tiene_zaps)
{
  if (sensores.agentes[2] != '_' || sensores.superficie[2] == 'M' || 
      sensores.superficie[2] == 'P' || sensores.superficie[2] == 'B') {
    return false;
  }

  if (sensores.cota[2] > sensores.cota[0]) {
    return false;
  }

  if (!es_camino(sensores.superficie[6])) {
    return false;
  }

  int diff_altura = abs(sensores.cota[6] - sensores.cota[0]);
  int limite = tiene_zaps ? 2 : 1; // <= 2 con zapatillas, <= 1 sin zapatillas

  if (diff_altura > limite) {
    return false;
  }

  return true;
}

char ComportamientoIngeniero::viablePorAlturaI(char casilla, int dif, bool zap)
{
  // Corregido para mayor legibilidad siguiendo el PDF
  if (abs(dif) <= 1 || (abs(dif) <= 2 && zap)) {
    return casilla;
  } else {
    return 'P';
  }
}

Action ComportamientoIngeniero::ComportamientoIngenieroNivel_0(Sensores sensores)
{
  Action accion;

  ActualizarMapa(sensores);

  // Si ya estamos en la planta de tratamiento, nos quedamos quietos
  if (sensores.superficie[0] == 'U') {
    return IDLE;
  }

  if (sensores.superficie[0] == 'D')
    tiene_zapatillas = true;

  if (visitas.empty())
  {
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
      sensores.posF == last_f && sensores.posC == last_c)
  {
    en_bloqueo = true;
    last_action = giro_preferido;
    return giro_preferido;
  }

  if ((last_action == JUMP) &&
      sensores.posF == last_f && sensores.posC == last_c)
  {
    en_bloqueo_J = true;
    last_action = giro_preferido;
    return giro_preferido;
  }

  char i = viablePorAlturaI(sensores.superficie[1], sensores.cota[1] - sensores.cota[0], tiene_zapatillas);
  char c = viablePorAlturaI(sensores.superficie[2], sensores.cota[2] - sensores.cota[0], tiene_zapatillas);
  char d = viablePorAlturaI(sensores.superficie[3], sensores.cota[3] - sensores.cota[0], tiene_zapatillas);

  int pos = veoCasillaInteresanteI0(i, c, d, tiene_zapatillas);

  if (sensores.superficie[0] == 'U')
  {
    if (c == 'U')
      return WALK;
    else if (d == 'U')
      return TURN_SR;
    else if (i == 'U')
      return TURN_SL;
    else
      return IDLE;
  }

  if ((en_bloqueo && c == 'U') || (en_bloqueo_J && sensores.superficie[6] == 'U'))
  {
    en_bloqueo_U = true;
    contador_giros++;
    last_action = giro_preferido;
    return giro_preferido;
  }

  if (en_bloqueo_U)
  {
    if (pos == 2 && en_bloqueo)
    {
      en_bloqueo_U = false;
      en_bloqueo = false;
      accion = WALK;
    }
    else if (en_bloqueo_J && puedeSaltarI(sensores, tiene_zapatillas))
    {
      en_bloqueo_U = false;
      en_bloqueo = false;
      accion = JUMP;
    }
    else
    {
      contador_giros++;
      accion = giro_preferido;
    }
    last_action = accion;
    return accion;
  }
  else if (en_bloqueo)
  {
    if (pos == 2)
    {
      en_bloqueo = false;
      accion = WALK;
    }
    else
      accion = giro_preferido;
    last_action = accion;
    return accion;
  }
  else if (en_bloqueo_J)
  {
    if (pos == 2)
    {
      en_bloqueo_J = false;
      accion = WALK;
    }
    else if (puedeSaltarI(sensores, tiene_zapatillas))
    {
      en_bloqueo_J = false;
      accion = JUMP;
    }
    else
      accion = giro_preferido;
    last_action = accion;
    return accion;
  }

  if (en_bloqueo && sensores.superficie[6] == 'U' && puedeSaltarI(sensores, tiene_zapatillas))
  {
    last_action = JUMP;
    return JUMP;
  }
  else
  {
    if (c == 'U')
    {
      accion = WALK;
      giro_defecto = false;
    }
    else if (d == 'U')
    {
      accion = TURN_SR;
      giro_defecto = false;
    }
    else if (i == 'U')
    {
      accion = TURN_SL;
      giro_defecto = false;
    }
    else if (es_camino(sensores.superficie[1]) && (i != 'P') &&
             visitas[pos_izq.f][pos_izq.c] < visitas[pos_frente.f][pos_frente.c])
    {
      accion = TURN_SL;
    }
    else if (es_camino(sensores.superficie[3]) && (d != 'P') &&
             visitas[pos_der.f][pos_der.c] < visitas[pos_frente.f][pos_frente.c])
    {
      accion = TURN_SR;
    }
    else if (pos == 2)
    {
      accion = WALK;
      giro_defecto = false;
    }
    else if (pos == 3)
    {
      accion = TURN_SR;
      giro_defecto = false;
    }
    else if (pos == 1)
    {
      accion = TURN_SL;
      giro_defecto = false;
    }
    else if (puedeSaltarI(sensores, tiene_zapatillas) && (pos == 0))
    {
      accion = JUMP;
      giro_defecto = false;
    }
    else
    {
      if (!giro_defecto)
      {
        accion = giro_preferido;
        giro_defecto = true;
      }
      else if (giro_preferido == TURN_SL)
        accion = TURN_SR;
      else
        accion = TURN_SL;
      contador_giros++;
    }

    if (contador_giros >= 15)
    {
      giro_preferido = (giro_preferido == TURN_SL) ? TURN_SR : TURN_SL;
      contador_giros = 0;
    }
  }

  if (accion == WALK)
  {
    cont_walk++;
    if (cont_walk >= 5)
    {
      if (walk_left)
      {
        accion = TURN_SL;
        walk_left = false;
      }
      else
      {
        accion = TURN_SR;
        walk_left = true;
      }
      cont_walk = 0;
      contador_giros++;
    }
  }
  else
    cont_walk = 0;

  last_f = sensores.posF;
  last_c = sensores.posC;
  last_action = accion;

  return accion;
}

Action ComportamientoIngeniero::ComportamientoIngenieroNivel_1(Sensores sensores)
{
  Action accion;

  ActualizarMapa(sensores);

  if (sensores.superficie[0] == 'D')
    tiene_zapatillas = true;

  if (visitas.empty())
  {
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
      sensores.posF == last_f && sensores.posC == last_c)
  {
    en_bloqueo = true;
    last_action = giro_preferido;
    return giro_preferido;
  }

  if ((last_action == JUMP) &&
      sensores.posF == last_f && sensores.posC == last_c)
  {
    en_bloqueo_J = true;
    last_action = giro_preferido;
    return giro_preferido;
  }

  char i = viablePorAlturaI(sensores.superficie[1], sensores.cota[1] - sensores.cota[0], tiene_zapatillas);
  char c = viablePorAlturaI(sensores.superficie[2], sensores.cota[2] - sensores.cota[0], tiene_zapatillas);
  char d = viablePorAlturaI(sensores.superficie[3], sensores.cota[3] - sensores.cota[0], tiene_zapatillas);

  int pos = veoCasillaInteresanteI1(i, c, d, tiene_zapatillas);

  if (en_bloqueo)
  {
    if (es_camino1(sensores.superficie[2]) && sensores.superficie[2] == c)
    {
      en_bloqueo = false;
      accion = WALK;
    }
    else
      accion = giro_preferido;
    last_action = accion;
    return accion;
  }
  else if (en_bloqueo_J)
  {
    if (es_camino1(sensores.superficie[2]) && sensores.superficie[2] == c)
    {
      en_bloqueo_J = false;
      accion = WALK;
    }
    else if (puedeSaltarI(sensores, tiene_zapatillas))
    {
      en_bloqueo_J = false;
      accion = JUMP;
    }
    else
      accion = giro_preferido;
    last_action = accion;
    return accion;
  }

  if (es_camino1(sensores.superficie[1]) && (i != 'P') &&
      visitas[pos_izq.f][pos_izq.c] < visitas[pos_frente.f][pos_frente.c])
  {
    accion = TURN_SL;
  }
  else if (es_camino1(sensores.superficie[3]) && (d != 'P') &&
           visitas[pos_der.f][pos_der.c] < visitas[pos_frente.f][pos_frente.c])
  {
    accion = TURN_SR;
  }
  else if (es_camino1(sensores.superficie[2]) && (c != 'P') && sensores.superficie[2] == c)
  {
    accion = WALK;
  }
  else if (pos == 2)
  {
    accion = WALK;
    giro_defecto = false;
  }
  else if (pos == 3)
  {
    accion = TURN_SR;
    giro_defecto = false;
  }
  else if (pos == 1)
  {
    accion = TURN_SL;
    giro_defecto = false;
  }
  else if (puedeSaltarI(sensores, tiene_zapatillas) && (pos == 0))
  {
    accion = JUMP;
    giro_defecto = false;
  }
  else
  {
    if (!giro_defecto)
    {
      accion = giro_preferido;
      giro_defecto = true;
    }
    else if (giro_preferido == TURN_SL)
      accion = TURN_SR;
    else
      accion = TURN_SL;
    contador_giros++;
  }

  if (contador_giros >= 15)
  {
    giro_preferido = (giro_preferido == TURN_SL) ? TURN_SR : TURN_SL;
    contador_giros = 0;
  }

  if (accion == WALK)
  {
    cont_walk++;
    if (cont_walk >= 4)
    {
      if (walk_left)
      {
        accion = TURN_SL;
        walk_left = false;
      }
      else
      {
        accion = TURN_SR;
        walk_left = true;
      }
      cont_walk = 0;
      contador_giros++;
    }
  }
  else
    cont_walk = 0;

  last_f = sensores.posF;
  last_c = sensores.posC;
  last_action = accion;

  return accion;
}

/////////////////////////////////////////////////////////
//NIVEL 2
/////////////////////////////////////////////////////////
EstadoI ComportamientoIngeniero::NextCasillaIngeniero(const EstadoI &st) const{
  EstadoI siguiente = st;
  switch (st.orientacion)
  {
  case norte:
    siguiente.fila = st.fila - 1;
    break;
  case noreste:
    siguiente.fila = st.fila - 1;
    siguiente.columna = st.columna + 1;
    break;
  case este:
    siguiente.columna = st.columna + 1;
    break;
  case sureste:
    siguiente.fila = st.fila + 1;
    siguiente.columna = st.columna + 1;
    break;
  case sur:
    siguiente.fila = st.fila + 1;
    break;
  case suroeste:
    siguiente.fila = st.fila + 1;
    siguiente.columna = st.columna - 1;
    break;
  case oeste:
    siguiente.columna = st.columna - 1;
    break;
  case noroeste:
    siguiente.fila = st.fila - 1;
    siguiente.columna = st.columna - 1;
    break;
  }
  return siguiente;
}
bool ComportamientoIngeniero::CasillaAccesibleJumpIngeniero(const EstadoI &st, const vector<vector<unsigned char>> &terreno, const vector<vector<unsigned char>> &altura) const{
  EstadoI inter = NextCasillaIngeniero(st);

  if (inter.fila < 0 || inter.fila >= terreno.size() || inter.columna < 0 || inter.columna >= terreno[0].size())
    return false;
  unsigned char sup_inter = terreno[inter.fila][inter.columna];

  if (sup_inter == 'P' || sup_inter == 'M' || sup_inter == 'B' || sup_inter == '?') // ? es para el nivel 6
    return false;

  if (altura[inter.fila][inter.columna] > altura[st.fila][st.columna])
    return false;

  EstadoI final_st = NextCasillaIngeniero(inter);
  if (final_st.fila < 0 || final_st.fila >= terreno.size() || final_st.columna < 0 || final_st.columna >= terreno[0].size())
    return false;
  unsigned char sup_final = terreno[final_st.fila][final_st.columna];

  if (sup_final == 'P' || sup_final == 'M' || sup_final == 'B' || sup_final == '?') // ? es para el nivel 6
    return false;

  int diff = altura[final_st.fila][final_st.columna] - altura[st.fila][st.columna];
  int limite_altura = st.zapatillas ? 2 : 1;

  return abs(diff) <= limite_altura;
}
EstadoI ComportamientoIngeniero::applyI(Action accion, const EstadoI &st, const vector<vector<unsigned char>> &terreno, const vector<vector<unsigned char>> &altura) const{
  EstadoI next = st;
  switch (accion)
  {
  case WALK:
    next = NextCasillaIngeniero(st);
    if (terreno[next.fila][next.columna] == 'D')
      next.zapatillas = true;
    break;
  case JUMP:
    next = NextCasillaIngeniero(NextCasillaIngeniero(st));
    if (terreno[next.fila][next.columna] == 'D')
      next.zapatillas = true;
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
list<Action> ComportamientoIngeniero::B_Anchura(const EstadoI &inicio, const EstadoI &final, const vector<vector<unsigned char>> &terreno, const vector<vector<unsigned char>> &altura){
  queue<NodoI> frontier;
  set<EstadoI> explored;
  list<Action> path;

  if (inicio.fila == final.fila && inicio.columna == final.columna)
    return path;
  
  NodoI current_node;
  current_node.estado = inicio;

  frontier.push(current_node);
  explored.insert(current_node.estado);

  while (!frontier.empty())
  {
    current_node = frontier.front();
    frontier.pop();

    // priorizamos JUMP porque avanza 2 casillas en 1
    Action acciones[] = {JUMP, WALK, TURN_SL, TURN_SR};

    for (Action accion : acciones)
    {
      bool posible = false;

      if (accion == WALK){
        posible = CasillaAccesibleIngeniero(current_node.estado, terreno, altura);
      }else if (accion == JUMP){
        posible = CasillaAccesibleJumpIngeniero(current_node.estado, terreno, altura);
      }else{
        posible = true; 
      }

      if (posible){
        NodoI child = current_node;
        child.estado = applyI(accion, current_node.estado, terreno, altura);
        child.secuencia.push_back(accion);

        if (child.estado.fila == final.fila && child.estado.columna == final.columna)
        {
          return child.secuencia; // ruta mas corta encontrada
        }

        // si es un estado no explorado lo añadimos
        if (explored.find(child.estado) == explored.end())
        {
          explored.insert(child.estado);
          frontier.push(child);
        }
      }
    }
  }

  return path; // si no hay camino devuelve lista vacía
}
Action ComportamientoIngeniero::ComportamientoIngenieroNivel_2(Sensores sensores)
{
  if (sensores.choque){
    if (sensores.agentes[2] == 't'){
      plan.push_front(last_action);
      return IDLE; 
    }
    else{
      hayPlan = false;
      plan.clear();
    }
  }

  if (!hayPlan){
    EstadoI inicio;
    inicio.fila = sensores.posF;
    inicio.columna = sensores.posC;
    inicio.orientacion = sensores.rumbo;

    if (sensores.superficie[0] == 'D')
      tiene_zapatillas = true;
    inicio.zapatillas = tiene_zapatillas;

    EstadoI meta;
    meta.fila = sensores.BelPosF;
    meta.columna = sensores.BelPosC;

    plan = B_Anchura(inicio, meta, mapaResultado, mapaCotas);
    hayPlan = (plan.size() > 0);

    if (hayPlan){
      ubicacion st_ini = {inicio.fila, inicio.columna, inicio.orientacion};
      VisualizaPlan(st_ini, plan);
    }
  }

  if (hayPlan && plan.size() > 0){
    Action accion_a_realizar = plan.front();
    plan.pop_front();

    if (plan.size() == 0)
      hayPlan = false;
    

    last_action = accion_a_realizar;
    return accion_a_realizar;
  }
  return IDLE;
}

/////////////////////////////////////////////////////////
//NIVEL 3
/////////////////////////////////////////////////////////
Action ComportamientoIngeniero::ComportamientoIngenieroNivel_3(Sensores sensores) { return IDLE; }

///////////////////////////////////////////////////////////////////////
// NIVEL 4
///////////////////////////////////////////////////////////////////////
bool ComportamientoIngeniero::TramoTuberiaValido(const EstadoTuberia &actual, int sig_fila, int sig_col, int sig_op, const std::vector<std::vector<unsigned char>> &terreno, const std::vector<std::vector<unsigned char>> &altura) const{
  if (sig_fila < 0 || sig_fila >= terreno.size() || sig_col < 0 || sig_col >= terreno[0].size())
    return false;
    
  unsigned char sup = terreno[sig_fila][sig_col];
  
  if (sup == 'P' || sup == 'M' || sup == '?')
    return false;

  int altura_original = altura[sig_fila][sig_col];

  if (sig_op == 1 && altura_original >= 9) return false;
  if (sig_op == -1 && altura_original <= 1) return false;
    
  int altura_tuberia_actual = altura[actual.fila][actual.columna] + actual.op;
  int altura_tuberia_siguiente = altura_original + sig_op;
  
  if (altura_tuberia_siguiente != altura_tuberia_actual && altura_tuberia_siguiente != (altura_tuberia_actual - 1))
    return false;
    
  if (sup == 'A' && sig_op != 0)
    return false;
    
  return true;
}
list<Paso> ComportamientoIngeniero::PlanificarRedTuberias(int inicioF, int inicioC, const std::vector<std::vector<unsigned char>> &terreno, const std::vector<std::vector<unsigned char>> &altura, int max_ecologico){
  priority_queue<NodoTuberia> frontier;
  map<EstadoTuberia, int> explored; 
  int operaciones[3] = {-1, 0, 1};

  for (int j = 0; j < 3; j++)
  {
    int o = operaciones[j];
    if (terreno[inicioF][inicioC] == 'A' && o != 0) continue;

    int altura_orig = altura[inicioF][inicioC];
    if (o == 1 && altura_orig >= 9) continue;
    if (o == -1 && altura_orig <= 1) continue;

    EstadoTuberia e_ini = {inicioF, inicioC, o};
    NodoTuberia n_ini;
    n_ini.estado = e_ini;
    n_ini.secuencia.push_back({inicioF, inicioC, o});
    n_ini.pasos = 1; // iniciar pasos

    // calculo impacto inicial
    int costo = 0;
    unsigned char sup = terreno[inicioF][inicioC];
    if (sup == 'A') costo = 50;
    else if (sup == 'H') costo = 45;
    else if (sup == 'S') costo = 25;
    else if (sup == 'C' || sup == 'U' || sup == 'D' || sup == 'X') costo = 15;
    else costo = 30; 

    if (o == 1) { 
      if (sup == 'H') costo += 55;
      else if (sup == 'S') costo += 30;
      else if (sup == 'C' || sup == 'U' || sup == 'D' || sup == 'X') costo += 10;
      else costo += 40;
    } else if (o == -1) { 
      if (sup == 'H') costo += 65;
      else if (sup == 'S') costo += 40;
      else if (sup == 'C' || sup == 'U' || sup == 'D' || sup == 'X') costo += 25;
      else costo += 50;
    }
    
    n_ini.costo = costo;

    if (n_ini.costo <= max_ecologico) {
      frontier.push(n_ini);
      explored[e_ini] = costo;
    }
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

    if (explored[actual] < nodoActual.costo) continue;

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

          // calculo del coste del tramo
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

          int nuevo_costo = nodoActual.costo + costo_tramo;

          // limite ecológico del nivel
          if (nuevo_costo > max_ecologico) continue;

          // guardar estado si es la primera vez, o si llegamos con un impacto <.
          if (explored.find(estado_hijo) == explored.end() || nuevo_costo < explored[estado_hijo])
          {
            explored[estado_hijo] = nuevo_costo;
            
            NodoTuberia hijo;
            hijo.estado = estado_hijo;
            hijo.secuencia = nodoActual.secuencia;
            hijo.secuencia.push_back({sig_fila, sig_col, sig_op});
            hijo.costo = nuevo_costo;
            hijo.pasos = nodoActual.pasos + 1; // + un paso

            frontier.push(hijo);
          }
        }
      }
    }
  }
  return std::list<Paso>();
}
Action ComportamientoIngeniero::ComportamientoIngenieroNivel_4(Sensores sensores)
{
  if (!hayPlan){
    planTuberias = PlanificarRedTuberias(sensores.BelPosF, sensores.BelPosC, mapaResultado, mapaCotas, sensores.max_ecologico);    
    VisualizaRedTuberias(planTuberias);
    hayPlan = true;
  }
  return IDLE;
}

///////////////////////////////////////////////////////////////////////
// NIVEL 5
///////////////////////////////////////////////////////////////////////
Action ComportamientoIngeniero::OrientarseHacia(Orientacion actual, Orientacion objetivo) const{
  if (actual == objetivo)
    return IDLE;

  int dist = (objetivo - actual + 8) % 8;

  if (dist <= 4)
    return TURN_SR;
  return TURN_SL;
}

Orientacion ComportamientoIngeniero::ObtenerOrientacionOrtogonal(const ubicacion &origen, const ubicacion &destino) const
{
  if ((destino.f - origen.f) == -1 && (destino.c - origen.c) == 0)
    return norte;
  else if ((destino.f - origen.f) == 0 && (destino.c - origen.c) == 1)
    return este;
  else if ((destino.f - origen.f) == 1 && (destino.c - origen.c) == 0)
    return sur;
  else
    return oeste;
}

ubicacion ComportamientoIngeniero::ElegirPosicionParaTecnico(int fila_ing, int col_ing, int fila_sig, int col_sig, const std::vector<std::vector<unsigned char>> &terreno, const std::vector<std::vector<unsigned char>> &altura) const {
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
  
Action ComportamientoIngeniero::ComportamientoIngenieroNivel_5(Sensores sensores)
{
  if (sensores.superficie[0] == 'D') tiene_zapatillas = true;

  // INICIALIZACIÓN: Planificar red
  if (!red_planificada) {
    planTuberias = PlanificarRedTuberias(sensores.BelPosF, sensores.BelPosC, mapaResultado, mapaCotas, sensores.max_ecologico);
    red_planificada = true; 
    
    if (planTuberias.empty()) {
      red_completada = true; 
      return IDLE;
    }
    tramo_actual = planTuberias.front();
    planTuberias.pop_front();
    estado_instalacion = 0;
    hayPlan = false;
    
    tramo_ant_f = -1;
    tramo_ant_c = -1;
  }

  if (red_completada) return IDLE;

  if (sensores.choque && estado_instalacion == 0) {
    if (sensores.agentes[2] == 't') {
      if (last_action != IDLE) plan.push_front(last_action);
      return IDLE; 
    } else {
      plan.clear();
      hayPlan = false; 
    }
  }

  if (estado_instalacion == 0) { // VIAJAR
    if (sensores.posF == tramo_actual.fil && sensores.posC == tramo_actual.col) {
      estado_instalacion = 1; 
    } else {
      if (!hayPlan) {
        EstadoI inicio = {sensores.posF, sensores.posC, sensores.rumbo, tiene_zapatillas};
        EstadoI final = {tramo_actual.fil, tramo_actual.col, norte, false};
        plan = B_Anchura(inicio, final, mapaResultado, mapaCotas);
        hayPlan = true;
      }
      if (!plan.empty()) {
        last_action = plan.front();
        plan.pop_front();
        return last_action;
      }
      return IDLE; 
    }
  }

  if (estado_instalacion == 1) { // DIG / RAISE
    if (tramo_actual.op == 1) {
      tramo_actual.op = 0; 
      estado_instalacion = 2; 
      last_action = RAISE;
      return RAISE;
    } else if (tramo_actual.op == -1) {
      tramo_actual.op = 0;
      estado_instalacion = 2;
      last_action = DIG;
      return DIG;
    }
    estado_instalacion = 2; 
  }

  if (estado_instalacion == 2) { // ALINEARSE Y VENPACA
    // si hay tuberia anterior vamos ahi si no calcular
    if (tramo_ant_f != -1) {
      pos_tecnico.f = tramo_ant_f;
      pos_tecnico.c = tramo_ant_c;
    } else {
      int f_sig = planTuberias.empty() ? -1 : planTuberias.front().fil;
      int c_sig = planTuberias.empty() ? -1 : planTuberias.front().col;
      pos_tecnico = ElegirPosicionParaTecnico(sensores.posF, sensores.posC, f_sig, c_sig, mapaResultado, mapaCotas);
    }

    if (pos_tecnico.f != -1) {
      ubicacion mi_pos = {sensores.posF, sensores.posC, sensores.rumbo};
      Orientacion ideal = ObtenerOrientacionOrtogonal(mi_pos, pos_tecnico);
      
      if (sensores.rumbo != ideal) { 
        last_action = OrientarseHacia(sensores.rumbo, ideal);
        return last_action;
      } else {
        estado_instalacion = 3;
        last_action = COME;
        return COME; 
      }
    }
    return IDLE; 
  }

  if (estado_instalacion == 3) { // INSTALAR
    if (sensores.agentes[2] == 't' && sensores.enfrente) {
      tramo_ant_f = tramo_actual.fil;
      tramo_ant_c = tramo_actual.col;

      if (!planTuberias.empty()) {
        tramo_actual = planTuberias.front();
        planTuberias.pop_front();
      } else {
        red_completada = true;
      }
      estado_instalacion = 0;
      hayPlan = false; 
      last_action = INSTALL;
      return INSTALL; 
    }
    
    last_action = COME;
    return COME; 
  }
  return IDLE;
}

///////////////////////////////////////////////////////////////////////
// NIVEL 6
///////////////////////////////////////////////////////////////////////
bool ComportamientoIngeniero::CasillaAccesibleIngeniero(const EstadoI &st, const vector<vector<unsigned char>> &terreno, const vector<vector<unsigned char>> &altura) const{
  EstadoI next = NextCasillaIngeniero(st);

  if (next.fila < 0 || next.fila >= terreno.size() || next.columna < 0 || next.columna >= terreno[0].size())
    return false;

  unsigned char sup = terreno[next.fila][next.columna];

  if (sup == 'P' || sup == 'M' || sup == 'B' || sup == '?') {
    return false;
  }

  int diff = altura[next.fila][next.columna] - altura[st.fila][st.columna];
  int limite_altura = st.zapatillas ? 2 : 1;

  return abs(diff) <= limite_altura;
}

Action ComportamientoIngeniero::ComportamientoIngenieroNivel_6(Sensores sensores)
{
  ActualizarMapa(sensores);
  if (sensores.superficie[0] == 'D') tiene_zapatillas = true;

  turnos_exploracion++; // t gastado explorando

  if (red_completada) return IDLE;

  // parte 1: EXPLORACIÓN + PLANIFICACIÓN
  if (!red_planificada) {
    int cantidad_u = 0;
    int min_dist_u = 9999;

    // solo evaluar distancias si ya sabemos donde esta la Belkanita
    if (mapaResultado[sensores.BelPosF][sensores.BelPosC] != '?') {
      
      // bucamos todas las plantas de tratamiento descubiertas en nuestro mapa
      for (int r = 0; r < (int)mapaResultado.size(); ++r) {
        for (int c = 0; c < (int)mapaResultado[0].size(); ++c) {
          if (mapaResultado[r][c] == 'U') {
            cantidad_u++;
            // calcular la distancia a la Belkanita
            int dist = abs(sensores.BelPosF - r) + abs(sensores.BelPosC - c);
            if (dist < min_dist_u) min_dist_u = dist;
          }
        }
      }

      // si hemos encontrado al menos una planta
      if (cantidad_u > 0) {
        // si la planta + cercana está lejos y no hemos explorado forzamos a seguir explorando para hallar una mejor.
        bool seguir_explorando = (min_dist_u > 15 && turnos_exploracion < 800);

        if (!seguir_explorando) {
          if (espera_tecnico <= 0) {
            planTuberias = PlanificarRedTuberias(sensores.BelPosF, sensores.BelPosC, mapaResultado, mapaCotas, sensores.max_ecologico);
            
            if (!planTuberias.empty()) {
              // EVALUAR IMPACTO ECOLÓGICO EXACTO DEL PLAN 
              int impacto_total = 0;
              for (auto const& paso : planTuberias) {
                unsigned char sup = mapaResultado[paso.fil][paso.col];
                int c_tramo = (sup == 'A')? 50 : (sup == 'H')? 45 : (sup == 'S')? 25 : (sup == 'C' || sup == 'U' || sup == 'D' || sup == 'X')? 15 : 30;
                if (paso.op == 1) c_tramo += (sup == 'H')? 55 : (sup == 'S')? 30 : (sup == 'C' || sup == 'U' || sup == 'D' || sup == 'X')? 10 : 40;
                else if (paso.op == -1) c_tramo += (sup == 'H')? 65 : (sup == 'S')? 40 : (sup == 'C' || sup == 'U' || sup == 'D' || sup == 'X')? 25 : 50;
                impacto_total += c_tramo;
              }

              // REGLA DE ACEPTACIÓN FINAL:
              // si el impacto es aceptable o si ya no hay tiempo 
              if (impacto_total < 800 || turnos_exploracion > 1200) {
                red_planificada = true;
                tramo_actual = planTuberias.front();
                planTuberias.pop_front();
                estado_instalacion = 0;
                hayPlan = false;
                tramo_ant_f = -1;
                tramo_ant_c = -1;
              } else {
                // rechazr el plan por alto impacto y aexplorar 50 turnos +
                planTuberias.clear();
                espera_tecnico = 50; 
              }
            } else {
              espera_tecnico = 20; // si A* falla por niebla exploramos 20 turnos
            }
          } else {
            espera_tecnico--;
          }
        }
      }
    }
    
    // si aun no hemos planificado seguimos explorando 
    if (!red_planificada) {
      Action act = ComportamientoIngenieroNivel_1(sensores);
      last_action = act;
      return act;
    }
  }

  // parte 2: CONSTRUCCIÓN + SINCRONIZACIÓN 
  if (sensores.choque && estado_instalacion == 0) {
    if (sensores.agentes[2] == 't') {
      if (last_action != IDLE) plan.push_front(last_action);
      last_action = IDLE;
      return IDLE; 
    } else {
      plan.clear();
      hayPlan = false; 
    }
  }

  if (estado_instalacion == 0) { // ir a la tuberia
    if (sensores.posF == tramo_actual.fil && sensores.posC == tramo_actual.col) {
      estado_instalacion = 1; 
    } else {
      if (!hayPlan) {
        EstadoI inicio = {sensores.posF, sensores.posC, sensores.rumbo, tiene_zapatillas};
        EstadoI final = {tramo_actual.fil, tramo_actual.col, norte, false};
        plan = B_Anchura(inicio, final, mapaResultado, mapaCotas);
        
        if (plan.empty()) { 
          // si bloqueo, girar 
          last_action = TURN_SR;
          return TURN_SR; 
        } else {
          hayPlan = true;
        }
      }
      if (!plan.empty()) {
        last_action = plan.front(); 
        plan.pop_front(); 
        return last_action;
      }
    }
  }

  if (estado_instalacion == 1) { // MODIFICAR TERRENO
    if (tramo_actual.op == 1) { 
      tramo_actual.op = 0; 
      estado_instalacion = 2; 
      last_action = RAISE;
      return RAISE; 
    } else if (tramo_actual.op == -1) { 
      tramo_actual.op = 0; 
      estado_instalacion = 2; 
      last_action = DIG;
      return DIG; 
    }
    estado_instalacion = 2; 
  }

  if (estado_instalacion == 2) { // mir pos tec y gritar 
    if (tramo_ant_f != -1) {
      pos_tecnico.f = tramo_ant_f; 
      pos_tecnico.c = tramo_ant_c;
    } else {
      int f_sig = planTuberias.empty() ? -1 : planTuberias.front().fil;
      int c_sig = planTuberias.empty() ? -1 : planTuberias.front().col;
      pos_tecnico = ElegirPosicionParaTecnico(sensores.posF, sensores.posC, f_sig, c_sig, mapaResultado, mapaCotas);
    }

    if (pos_tecnico.f != -1) {
      ubicacion mi_pos = {sensores.posF, sensores.posC, sensores.rumbo};
      Orientacion ideal = ObtenerOrientacionOrtogonal(mi_pos, pos_tecnico);
      
      if (sensores.rumbo != ideal) { 
        last_action = OrientarseHacia(sensores.rumbo, ideal);
        return last_action; 
      } else {
        estado_instalacion = 3;
        last_action = COME;
        return COME; 
      }
    } else {
      last_action = TURN_SR; // si está rodeado de ? gira para mapear
      return TURN_SR; 
    }
  }

  if (estado_instalacion == 3) { // esperar al tec e instalar
    if (sensores.agentes[2] == 't' && sensores.enfrente) {
      tramo_ant_f = tramo_actual.fil; 
      tramo_ant_c = tramo_actual.col;
      if (!planTuberias.empty()) {
        tramo_actual = planTuberias.front(); 
        planTuberias.pop_front();
      } else { 
        red_completada = true; 
      }
      estado_instalacion = 0; 
      hayPlan = false; 
      last_action = INSTALL;
      return INSTALL; 
    }
    
    last_action = COME;
    return COME; 
  }

  return IDLE;
}


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
      for (int i = -j; i <= j; i++)
      {
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
      for (int i = -j; i <= j; i++)
      {
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
      for (int i = -j; i <= j; i++)
      {
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
      for (int i = -j; i <= j; i++)
      {
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

bool ComportamientoIngeniero::EsCasillaTransitableLevel0(int f, int c, bool tieneZapatillas)
{
  if (f < 0 || f >= (int)mapaResultado.size() || c < 0 || c >= (int)mapaResultado[0].size())
    return false;
  return es_camino(mapaResultado[f][c]);
}

bool ComportamientoIngeniero::EsAccesiblePorAltura(const ubicacion &actual, bool zap)
{
  ubicacion del = Delante(actual);
  if (del.f < 0 || del.f >= (int)mapaCotas.size() || del.c < 0 || del.c >= (int)mapaCotas[0].size())
    return false;
  int desnivel = abs(mapaCotas[del.f][del.c] - mapaCotas[actual.f][actual.c]);
  if (zap && desnivel > 2)
    return false;
  if (!zap && desnivel > 1)
    return false;
  return true;
}

ubicacion ComportamientoIngeniero::Delante(const ubicacion &actual) const
{
  ubicacion delante = actual;
  switch (actual.brujula)
  {
  case 0:
    delante.f--;
    break;
  case 1:
    delante.f--;
    delante.c++;
    break;
  case 2:
    delante.c++;
    break;
  case 3:
    delante.f++;
    delante.c++;
    break;
  case 4:
    delante.f++;
    break;
  case 5:
    delante.f++;
    delante.c--;
    break;
  case 6:
    delante.c--;
    break;
  case 7:
    delante.f--;
    delante.c--;
    break;
  }
  return delante;
}

void ComportamientoIngeniero::PintaPlan(const list<Action> &plan)
{
  auto it = plan.begin();
  while (it != plan.end())
  {
    if (*it == WALK)
      cout << "W ";
    else if (*it == JUMP)
      cout << "J ";
    else if (*it == TURN_SR)
      cout << "r ";
    else if (*it == TURN_SL)
      cout << "l ";
    else if (*it == COME)
      cout << "C ";
    else if (*it == IDLE)
      cout << "I ";
    else
      cout << "-_ ";
    it++;
  }
  cout << "( longitud " << plan.size() << ")" << endl;
}

void ComportamientoIngeniero::PintaPlan(const list<Paso> &plan)
{
  auto it = plan.begin();
  while (it != plan.end())
  {
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
      if (cst.f >= 0 && cst.f < (int)mapaResultado.size() &&
          cst.c >= 0 && cst.c < (int)mapaResultado[0].size())
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
    default:
      break;
    }
    it++;
  }
}

void ComportamientoIngeniero::VisualizaRedTuberias(const list<Paso> &plan)
{
  listaCanalizacionTuberias.clear();
  auto it = plan.begin();
  while (it != plan.end())
  {
    listaCanalizacionTuberias.push_back({it->fil, it->col, it->op});
    it++;
  }
}
