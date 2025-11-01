#include <string>
#include <iostream>
#include <string.h> 
#include <list>
#include <map>
#include <fstream>
#include <cctype> 
#include <vector>
#include <cstdlib>
#include <utility> 
#include <stack> 
#include <sstream> //new
#include <emscripten/emscripten.h>


using namespace std;
static string bufferGlobal;

// ================================================================
// =============== TOKENS PRINCIPALES DEL LENGUAJE ================
// ================================================================

// ---------- [100–199] Palabras clave principales ----------
#define IF                      101
#define ELSE                    102
#define FOR_EACH_AGENT          157
#define WHERE                   156
#define STOP                    149
#define TRUE                    153
#define FALSE                   154

#define AGENT                   120
#define NETWORK                 121
#define DYNAMICS                122
#define METRICS                 123
#define EXPERIMENT              124
#define TRIGGER                 125
#define RUN                     126
#define GLOBALS                 127

#define APPLY_NETWORK           128
#define APPLY_DYNAMICS          129
#define APPLY_METRICS           130
#define TICKS        131

#define EVERY                   132
#define AT_END                  133
#define AT_TICK                 134
#define ON_CONDITION            135
#define RESULT_SET              136
#define GRAPH                   137
#define COLLECT                 138
#define AS                      139
#define TICK                    140
#define DURATION                141
#define PROPS                   142
#define ON_INTERACTION          144
#define ON_START                145
#define AGENTS                  146
#define TOPOLOGY                147
#define LINK_PROPS              148
#define FOR_KEYWORD             157
#define RUN_EXPERIMENT          158
#define PRINT                   159
#define TOP                     160

// ---------- [161–199] Dinámicas, métricas y acciones ----------
#define BETWEENNESS_CENTRALITY  161
#define INTERACT                 164
#define BROADCAST                165
#define GET_LINK_PROP            167
#define SET_LINK_PROP            168
#define DEGREE                   169
#define FIND_NEIGHBOR_SIMILAR    170
#define FIND_NEIGHBOR_DISSIMILAR 171
#define FIND_NEIGHBOR_MAX        172
#define FIND_NEIGHBOR_MIN        173
#define RANDOM_CHOICE            175
#define TARGET                   176
#define MEDIAN                   177
#define VARIANCE                 178
#define MODE                     179
#define MIN                      180
#define MAX                      181
#define RANGE                    182
#define SKEWNESS                 183
#define KURTOSIS                 184
#define ALL_PROPS                185
#define AVG_LINK_PROP            186
#define MODULARITY               187
#define POLARIZATION_ER          188
#define EXPORT_CSV               189
#define NORMAL                   190
#define CLOSENESS_CENTRALITY     191
#define ARE_CONNECTED            192
#define RANDOM_NEIGHBOR          193

#define ON_AGENT_STEP            194
#define ON_BROADCAST             195
#define ON_WORLD_STEP            196

// ---------- [150–155] Operaciones estadísticas y aleatorias ----------
#define RANDOM                   150
#define AVG                      151
#define STDDEV                   152
#define COUNT                    153
#define CLUSTERING_COEFFICIENT   154
#define INJECT                   155

// ================================================================
// ===================== OPERADORES [200–219] =====================
// ================================================================
#define SUMA            201
#define RESTA           202
#define MULT            203
#define DIV             204
#define ASIGN           205  // =
#define IGUAL           206  // ==
#define DISTINTO        207  // !=
#define MENOR           208  // <
#define MAYOR           209  // >
#define MENOR_IGUAL     210  // <=
#define MAYOR_IGUAL     211  // >=
#define OP_OR           212  // ||
#define OP_AND          213  // &&
// NOT (!) se maneja como símbolo especial si se necesita

// ================================================================
// ================== SÍMBOLOS ESPECIALES [300–310] ===============
// ================================================================
#define PUNTOYCOMA      301
#define PAREN_ABRE      302
#define PAREN_CIERRA    303
#define LLAVE_ABRE      304
#define LLAVE_CIERRA    305
#define COMA            306
#define PUNTO           307
#define DOS_PUNTOS      308
#define CORCHE_ABRE     309
#define CORCHE_CIERRA   310

// ================================================================
// ================== LITERALES E IDENTIFICADORES =================
// ================================================================
#define IDENTIFICADOR   400
#define NUMERO          401
#define CADENA          402
#define DECIMAL         403
#define AGENT_TYPE      404
#define SERIES      405
#define TITLE      406
#define FILENAME      407
// ================================================================
// ===================== ESPECIALES Y CONTROL =====================
// ================================================================
#define FIN             666
#define ERROR           999

// ================================================================
// ===================== ESTADOS DE PARSEO ========================
// ================================================================
#define ESTADO_INICIAL      0
#define ESTADO_AGENT        108
#define ESTADO_NETWORK      102
#define ESTADO_DYNAMICS     103
#define ESTADO_METRICS      104
#define ESTADO_EXPERIMENT   105
#define ESTADO_RUN          106
#define ESTADO_GLOBALS      107
#define ESTADO_FINAL        999

// ================================================================
// ==================== CONSTANTES SIMBÓLICAS =====================
// ================================================================
#define null    "NULL"
#define vacio   "-"

// ================================================================
// ========== RANGOS RESERVADOS PARA EXPANSIONES FUTURAS ==========
// ================================================================
// 220–249 → Operadores lógicos, aritméticos avanzados o bitwise
// 250–299 → Nuevas palabras clave o estructuras de control
// 311–399 → Nuevos símbolos o delimitadores personalizados
// 404–499 → Nuevos tipos de literales o tokens semánticos
// 500–599 → Extensiones para módulos o plugins
// 600–665 → Reservado para futuras fases del parser

// ---------------- CLASES AUXILIARES ----------------
class Atributos {
public:
    string lexema;
    int token;
    string tipo;    // "pclave", "agent", "network", ..., "globals", "variable"
    string valor;
    string estado;

    Atributos() {
        lexema = "";
        token = -1;
        tipo = "";
        valor = "";
        estado = "";
    }
    Atributos(string lex, int tok, string tip, string val, string est) {
        lexema = lex;
        token = tok;
        tipo = tip;
        valor = val;
        estado = est;
    }

    void Mostrar() {
        cout << "Tipo(" << tipo << ")\t";
        cout << "Lexema(" << lexema << ")\t";
        cout << "Token(" << token << ")\t";
        cout << "Valor(" << valor << ")\t";
        cout << "Estado(" << estado << ")" << endl;
    }
};


class TablaSimbolos {
private:
    list<Atributos> tabla;
public:
    // Insertar, ActualizarValor, ActualizarTipo, ActualizarEstado, Mostrar, Buscar, Existe, getTabla SIN CAMBIOS
    void Insertar(string lex,int tok,string tip,string val,string est){
        Atributos attr(lex,tok,tip,val,est);
        tabla.push_back(attr);
    }
    bool ActualizarValor(string lex,string val){ 
        for (auto &item:tabla){
            if(item.lexema == lex){
                item.valor = val;
                return true;
            }
        }
        return false;
    }
    bool ActualizarTipo(string lex,string tipo){ 
        for (auto &item:tabla){
            if(item.lexema == lex){
                item.tipo = tipo;
                return true;
            }
        }
        return false;
    }
    bool ActualizarEstado(string lex,string est){ /* ... sin cambios ... */
        for (auto &item:tabla){
            if(item.lexema == lex){
                item.estado = est;
                return true;
            }
        }
        return false;
    }
    void Mostrar(){ /* ... sin cambios ... */
        for (auto item:tabla){
            item.Mostrar();
        }
    }
    bool Buscar(string lex,Atributos&attr){ /* ... sin cambios ... */
        for (auto item:tabla){
            if(item.lexema == lex){
                attr=item;
                return true;
            }
        }
        return false;
    }
    // BuscarPClave renombrado a BuscarPorTipo SIN CAMBIOS en lógica
    bool BuscarPorTipo(string lex,string tipo,Atributos&attr){
        for (auto item:tabla){
            if(item.lexema == lex && item.tipo == tipo){
                attr=item;
                return true;
            }
        }
        return false;
    }
    bool Existe(string lex){ /* ... sin cambios ... */
        for (auto item:tabla) if (item.lexema == lex) return true;
        return false;
    }
    list<Atributos> getTabla(){ /* ... sin cambios ... */
        return tabla;
    }
};

// ---------------- ANALIZADOR LÉXICO (Versión Sincronizada con Tokens) ----------------
class AnalizadorLexico {
private:
    int i; // Apuntador al caracter actual
    char cad[8192]; // Aumentado tamaño por si acaso
    TablaSimbolos ts_claves; // Tabla solo para palabras clave
    

public:
    string numero;
    string variable;
    string cadena;
    
    bool cargarCodigo(const string& codigo) {
        strncpy(cad, codigo.c_str(), sizeof(cad)-1);
        cad[sizeof(cad)-1] = '\0'; // null terminate
        i = 0; // reiniciar índice de análisis
        return true;
        
    }

    AnalizadorLexico() {
        i = 0;
        cad[0] = '\0';

        // -------------------------------------------------------------------
        // Poblar tabla ts_claves con TODAS las palabras clave y funciones integradas
        // -------------------------------------------------------------------
        ts_claves.Insertar("agent", AGENT, "pclave", vacio, vacio);
        ts_claves.Insertar("network", NETWORK, "pclave", vacio, vacio);
        ts_claves.Insertar("dynamics", DYNAMICS, "pclave", vacio, vacio);
        ts_claves.Insertar("metrics", METRICS, "pclave", vacio, vacio);
        ts_claves.Insertar("experiment", EXPERIMENT, "pclave", vacio, vacio);
        ts_claves.Insertar("trigger", TRIGGER, "pclave", vacio, vacio);
        ts_claves.Insertar("run", RUN, "pclave", vacio, vacio);
        ts_claves.Insertar("globals", GLOBALS, "pclave", vacio, vacio);

        ts_claves.Insertar("apply_network", APPLY_NETWORK, "pclave", vacio, vacio);
        ts_claves.Insertar("apply_dynamics", APPLY_DYNAMICS, "pclave", vacio, vacio);
        ts_claves.Insertar("apply_metrics", APPLY_METRICS, "pclave", vacio, vacio);
        ts_claves.Insertar("ticks", TICKS, "pclave", vacio, vacio);

        ts_claves.Insertar("props", PROPS, "pclave", vacio, vacio);
        ts_claves.Insertar("agents", AGENTS, "pclave", vacio, vacio);
        ts_claves.Insertar("topology", TOPOLOGY, "pclave", vacio, vacio);
        ts_claves.Insertar("link_props", LINK_PROPS, "pclave", vacio, vacio);
        ts_claves.Insertar("duration", DURATION, "pclave", vacio, vacio);

        // Eventos
        ts_claves.Insertar("on_start", ON_START, "pclave", vacio, vacio);
        ts_claves.Insertar("on_interaction", ON_INTERACTION, "pclave", vacio, vacio);
        ts_claves.Insertar("on_agent_step", ON_AGENT_STEP, "pclave", vacio, vacio);
        ts_claves.Insertar("on_broadcast", ON_BROADCAST, "pclave", vacio, vacio);
        ts_claves.Insertar("on_world_step", ON_WORLD_STEP, "pclave", vacio, vacio);

        // Control de flujo
        ts_claves.Insertar("if", IF, "pclave", vacio, vacio);
        ts_claves.Insertar("else", ELSE, "pclave", vacio, vacio);
        ts_claves.Insertar("for_each_agent", FOR_EACH_AGENT, "pclave", vacio, vacio);
        ts_claves.Insertar("where", WHERE, "pclave", vacio, vacio);
        ts_claves.Insertar("stop", STOP, "pclave", vacio, vacio);
        ts_claves.Insertar("true", TRUE, "pclave", vacio, vacio);
        ts_claves.Insertar("false", FALSE, "pclave", vacio, vacio);

        // Reglas y ciclos
        ts_claves.Insertar("collect", COLLECT, "pclave", vacio, vacio);
        ts_claves.Insertar("as", AS, "pclave", vacio, vacio);
        ts_claves.Insertar("every", EVERY, "pclave", vacio, vacio);
        ts_claves.Insertar("at_end", AT_END, "pclave", vacio, vacio);
        ts_claves.Insertar("at_tick", AT_TICK, "pclave", vacio, vacio);
        ts_claves.Insertar("on_condition", ON_CONDITION, "pclave", vacio, vacio);
        ts_claves.Insertar("result_set", RESULT_SET, "pclave", vacio, vacio);
        ts_claves.Insertar("tick", TICK, "pclave", vacio, vacio);

        // Funciones estadísticas y de red
        ts_claves.Insertar("graph", GRAPH, "pclave", vacio, vacio);
        ts_claves.Insertar("random", RANDOM, "pclave", vacio, vacio);
        ts_claves.Insertar("avg", AVG, "pclave", vacio, vacio);
        ts_claves.Insertar("stddev", STDDEV, "pclave", vacio, vacio);
        ts_claves.Insertar("count", COUNT, "pclave", vacio, vacio);
        ts_claves.Insertar("clustering_coefficient", CLUSTERING_COEFFICIENT, "pclave", vacio, vacio);
        ts_claves.Insertar("inject", INJECT, "pclave", vacio, vacio);
        ts_claves.Insertar("run_experiment", RUN_EXPERIMENT, "pclave", vacio, vacio);
        ts_claves.Insertar("print", PRINT, "pclave", vacio, vacio);
        ts_claves.Insertar("top", TOP, "pclave", vacio, vacio);
        ts_claves.Insertar("betweenness_centrality", BETWEENNESS_CENTRALITY, "pclave", vacio, vacio);
        ts_claves.Insertar("interact", INTERACT, "pclave", vacio, vacio);
        ts_claves.Insertar("broadcast", BROADCAST, "pclave", vacio, vacio);
        ts_claves.Insertar("get_link_prop", GET_LINK_PROP, "pclave", vacio, vacio);
        ts_claves.Insertar("set_link_prop", SET_LINK_PROP, "pclave", vacio, vacio);
        ts_claves.Insertar("degree", DEGREE, "pclave", vacio, vacio);
        ts_claves.Insertar("find_neighbor_similar", FIND_NEIGHBOR_SIMILAR, "pclave", vacio, vacio);
        ts_claves.Insertar("find_neighbor_dissimilar", FIND_NEIGHBOR_DISSIMILAR, "pclave", vacio, vacio);
        ts_claves.Insertar("find_neighbor_max", FIND_NEIGHBOR_MAX, "pclave", vacio, vacio);
        ts_claves.Insertar("find_neighbor_min", FIND_NEIGHBOR_MIN, "pclave", vacio, vacio);
        ts_claves.Insertar("random_choice", RANDOM_CHOICE, "pclave", vacio, vacio);
        ts_claves.Insertar("target", TARGET, "pclave", vacio, vacio);
        ts_claves.Insertar("median", MEDIAN, "pclave", vacio, vacio);
        ts_claves.Insertar("variance", VARIANCE, "pclave", vacio, vacio);
        ts_claves.Insertar("mode", MODE, "pclave", vacio, vacio);
        ts_claves.Insertar("min", MIN, "pclave", vacio, vacio);
        ts_claves.Insertar("max", MAX, "pclave", vacio, vacio);
        ts_claves.Insertar("range", RANGE, "pclave", vacio, vacio);
        ts_claves.Insertar("skewness", SKEWNESS, "pclave", vacio, vacio);
        ts_claves.Insertar("kurtosis", KURTOSIS, "pclave", vacio, vacio);
        ts_claves.Insertar("all_props", ALL_PROPS, "pclave", vacio, vacio);
        ts_claves.Insertar("avg_link_prop", AVG_LINK_PROP, "pclave", vacio, vacio);
        ts_claves.Insertar("modularity", MODULARITY, "pclave", vacio, vacio);
        ts_claves.Insertar("polarization_er", POLARIZATION_ER, "pclave", vacio, vacio);
        ts_claves.Insertar("export_csv", EXPORT_CSV, "pclave", vacio, vacio);
        ts_claves.Insertar("normal", NORMAL, "pclave", vacio, vacio);
        ts_claves.Insertar("closeness_centrality", CLOSENESS_CENTRALITY, "pclave", vacio, vacio);
        ts_claves.Insertar("are_connected", ARE_CONNECTED, "pclave", vacio, vacio);
        ts_claves.Insertar("random_neighbor", RANDOM_NEIGHBOR, "pclave", vacio, vacio);
        ts_claves.Insertar("agent_type", AGENT_TYPE, "pclave", vacio, vacio);
        ts_claves.Insertar("series", SERIES, "pclave", vacio, vacio);
        ts_claves.Insertar("title", TITLE, "pclave", vacio, vacio);
        ts_claves.Insertar("filename", FILENAME, "pclave", vacio, vacio);
    }

    // -------------------------------------------------------------------
    // Lectura del archivo fuente
    // -------------------------------------------------------------------
    bool leerArchivo(const char* direccion) {
        ifstream archivo(direccion);
        if (!archivo.is_open()) return false;
        archivo.read(cad, sizeof(cad) - 1);
        cad[archivo.gcount()] = '\0';
        archivo.close();
        i = 0;
        return true;
    }

    // -------------------------------------------------------------------
    // Obtención de tokens
    // -------------------------------------------------------------------
    int getToken() {
        while (isspace(cad[i])) i++;
        if (cad[i] == '\0') return FIN;

        // Comentarios //
        if (cad[i] == '/' && cad[i + 1] == '/') {
            while (cad[i] != '\n' && cad[i] != '\0') i++;
            return getToken();
        }

        // Cadenas
        if (cad[i] == '"') {
            i++;
            string lex = "";
            while (cad[i] != '"' && cad[i] != '\0') {
                if (cad[i] == '\\' && cad[i + 1] == '"') {
                    lex += '"'; i += 2;
                } else {
                    lex += cad[i]; i++;
                }
            }
            if (cad[i] == '"') i++;
            cadena = lex;
            return CADENA;
        }

        // Identificadores y palabras clave
        if (isalpha(cad[i]) || cad[i] == '_') {
            string lex = "";
            while (isalnum(cad[i]) || cad[i] == '_') { lex += cad[i]; i++; }
            Atributos attr;
            if (ts_claves.BuscarPorTipo(lex, "pclave", attr)) return attr.token;
            variable = lex;
            return IDENTIFICADOR;
        }

        // Números
        if (isdigit(cad[i]) || (cad[i] == '-' && isdigit(cad[i + 1])) || (cad[i] == '.' && isdigit(cad[i + 1]))) {
            string lex = "";
            bool esDecimal = false;
            if (cad[i] == '-') { lex += cad[i]; i++; }
            while (isdigit(cad[i]) || (cad[i] == '.' && !esDecimal)) {
                if (cad[i] == '.') esDecimal = true;
                lex += cad[i]; i++;
            }
            numero = lex;
            return esDecimal ? DECIMAL : NUMERO;
        }

        // Operadores dobles
        string lex = "";
        lex += cad[i];
        if (i + 1 < (int)strlen(cad)) {
            string lex2 = lex + cad[i + 1];
            if (lex2 == "==") { i += 2; return IGUAL; }
            if (lex2 == "!=") { i += 2; return DISTINTO; }
            if (lex2 == "<=") { i += 2; return MENOR_IGUAL; }
            if (lex2 == ">=") { i += 2; return MAYOR_IGUAL; }
            if (lex2 == "||") { i += 2; return OP_OR; }
            if (lex2 == "&&") { i += 2; return OP_AND; }
        }

        // Operadores y símbolos simples
        i++;
        if (lex == "+") return SUMA;
        if (lex == "-") return RESTA;
        if (lex == "*") return MULT;
        if (lex == "/") return DIV;
        if (lex == "=") return ASIGN;
        if (lex == "<") return MENOR;
        if (lex == ">") return MAYOR;
        if (lex == ";") return PUNTOYCOMA;
        if (lex == ":") return DOS_PUNTOS;
        if (lex == "(") return PAREN_ABRE;
        if (lex == ")") return PAREN_CIERRA;
        if (lex == "[") return CORCHE_ABRE;
        if (lex == "]") return CORCHE_CIERRA;
        if (lex == "{") return LLAVE_ABRE;
        if (lex == "}") return LLAVE_CIERRA;
        if (lex == ",") return COMA;
        if (lex == ".") return PUNTO;
        if (lex == "!") return DISTINTO; // opcional para negación simple

        return ERROR;
    }
};





// ---------------- ANALIZADOR SINTÁCTICO CON TABLA DE TRANSICIONES (Extendida) ----------------
class AnalizadorSintactico {
private:
    AnalizadorLexico &lex;
    int token;
    TablaSimbolos ts_usuario; // Tabla para definiciones de usuario
    string lastIdent;
    string lastNumber;
    string lastString;
    stack<int> pila;
    
    // --- NUEVAS CONSTANTES para la Pila ---
    // Usaremos números negativos para acciones especiales
    static const int ESTADO_RETORNO = -9;
    static const int MARCADOR_BLOQUE = -10;

    // --- NUEVOS ESTADOS COMPARTIDOS (p.ej., rango 200+) ---
    static const int ESTADO_RANDOM_INICIO = 200;
    static const int ESTADO_DEGREE_INICIO = 210;
    static const int ESTADO_NORMAL_INICIO = 220;
    static const int ESTADO_UNIFORM_INICIO = 230;
    static const int ESTADO_INJECT_INICIO = 239;
    
    // Estructura para guardar contexto de bloques
    struct ContextoBloque {
        string tipoBloque;    // "AGENT", "GLOBALS", "PROPS", "DYNAMICS", etc.
        int estadoRetorno;    // Estado al que regresar al cerrar }
        string nombreBloque;  // Nombre opcional (ej: "Comprador" para agent)
        
        ContextoBloque(string tipo, int estado, string nombre = "")
            : tipoBloque(tipo), estadoRetorno(estado), nombreBloque(nombre) {}
    };
    //Pila para contexto
    stack<ContextoBloque> pilaContextos;
    
    // ========== TABLA DE TRANSICIONES (Matriz 2D Extendida) ==========
    int estadoActual;
    // Aumentamos tamaño para más estados
    int tablaTransicion[1000][1000]; // Tamaño [Estados][Tokens+1]

    void inicializarTablaTransicion() {
        // Inicializar toda la tabla con ERROR
        for(int i = 0; i < 500; i++) { // Límite de estados aumentado
            for(int j = 0; j < 410; j++) { // Límite de tokens
                tablaTransicion[i][j] = ERROR;
            }
        }

        // ========== TRANSICIONES PRINCIPALES (Bloques de alto nivel) ==========
        tablaTransicion[ESTADO_INICIAL][GLOBALS] = ESTADO_GLOBALS; // Va a estado 107 (GLOBALS)
        tablaTransicion[ESTADO_INICIAL][AGENT] = ESTADO_AGENT;     // Va a estado 108
        tablaTransicion[ESTADO_INICIAL][NETWORK] = ESTADO_NETWORK; // Va a estado 102
        tablaTransicion[ESTADO_INICIAL][DYNAMICS] = ESTADO_DYNAMICS; // Va a estado 103
        tablaTransicion[ESTADO_INICIAL][METRICS] = ESTADO_METRICS;   // Va a estado 104
        tablaTransicion[ESTADO_INICIAL][EXPERIMENT] = ESTADO_EXPERIMENT;   // Va a estado 104
        tablaTransicion[ESTADO_INICIAL][RUN] = ESTADO_RUN;   // Va a estado 104

        // ================================================================
        // ========== TRANSICIONES PARA AGENT (Estados 2-20) ==========
        // Sintaxis: agent <id> { props: { (<id> : <valor>)* } }
        // ================================================================
        // E108 (ESTADO_AGENT) -> E2 : 'agent' ... espera ID
        tablaTransicion[ESTADO_AGENT][IDENTIFICADOR] = 2;

        // E2 -> E3 : 'agent ID' ... espera '{'
        tablaTransicion[2][LLAVE_ABRE] = 3;



        // E3 -> E4 (props) o E0 (agente vacío) : 'agent ID {' ... espera 'props' o '}'
        tablaTransicion[3][PROPS] = 4;
        tablaTransicion[3][LLAVE_CIERRA] = ESTADO_INICIAL; // Permite 'agent Foo {}'

        // E4 -> E5 : 'agent ID { props' ... espera ':'
        tablaTransicion[4][DOS_PUNTOS] = 5;

        // E5 -> E6 : 'agent ID { props:' ... espera '{'
        tablaTransicion[5][LLAVE_ABRE] = 6;

        // E6 -> E8 (ID) o E7 (fin props vacías) : '... props: {' ... espera ID de prop o '}'
        tablaTransicion[6][IDENTIFICADOR] = 8;
        tablaTransicion[6][LLAVE_CIERRA] = 7; // Permite 'props: {}'

        // E8 -> E10 : '... props: { mi_prop' ... espera ':'
        tablaTransicion[8][DOS_PUNTOS] = 10;

        // E10 (Esperando <valor>) -> E9 (Valor simple) / E11 (Neg) / E12 (random) / E13 (degree)
        // ... 'props: { mi_prop:' ... espera <valor>
        tablaTransicion[10][NUMERO]       = 9; // Valor asignado
        tablaTransicion[10][DECIMAL]      = 9; // Valor asignado
        tablaTransicion[10][CADENA]       = 9; // Valor asignado
        tablaTransicion[10][TRUE]         = 9; // Valor asignado
        tablaTransicion[10][FALSE]        = 9; // Valor asignad
        tablaTransicion[10][RESTA]        = 11; // Inicio de valor negativo
        tablaTransicion[11][NUMERO] = 9;
        tablaTransicion[11][DECIMAL] = 9;
        
        // E9 -> E6 (bucle) o E7 (fin) : '...: <valor>' ... espera ',' o '}'
        tablaTransicion[9][COMA] = 6;       // Bucle a esperar siguiente ID de prop
        tablaTransicion[9][LLAVE_CIERRA] = 7; // Fin del bloque props

        // E7 -> E0 : '... props: {...}' ... espera '}' de cierre del agente
        tablaTransicion[7][LLAVE_CIERRA] = ESTADO_INICIAL; // Fin del bloque 'agent'
        
        // ================================================================
        // ========== TRANSICIONES PARA NETWORK (Estados 21-46) ==========
        /* Sintaxis: network <id>{
                        agents: [<id>[<cantidad>],]*;
                        topology: (<id>(argumentos)); 
                        link_props: {(<propiedad>: <valor>)*}; | -> establece propiedades a cada conexion
                        }
        */
        // ================================================================
        // ESTADO_NETWORK (102) -> E21: 'network' ... espera ID
        tablaTransicion[ESTADO_NETWORK][IDENTIFICADOR] = 21;
        // E21 -> E22: 'network ID' ... espera '{'
        tablaTransicion[21][LLAVE_ABRE] = 22;
        // E22 -> E23 | E0: '{ ... espera 'agents' o '}' (network vacío)
        tablaTransicion[22][AGENTS] = 23;
        tablaTransicion[22][LLAVE_CIERRA] = ESTADO_INICIAL; // Permite network vacío
        
        
        // ========== SECCIÓN: agents (Estados 23-30) ==========
        // E23 -> E24: 'agents' ... espera ':'
        tablaTransicion[23][DOS_PUNTOS] = 24;
        tablaTransicion[24][IDENTIFICADOR] = 25;
        // E26 -> E27: 'agents: ID' ... espera '['
        tablaTransicion[25][CORCHE_ABRE] = 26;
        // E27 -> E28: 'agents: ID[' ... espera NUMERO (cantidad)
        tablaTransicion[26][NUMERO] = 27;
        // E28 -> E29: 'agents: [ ID[NUM' ... espera ']'
        tablaTransicion[27][CORCHE_CIERRA] = 28;
        // E29 -> E25 | E30: 'agents: [ ID[NUM]' ... espera ',' (más agentes) o ']' (fin)
        tablaTransicion[28][COMA] = 24;         // Bucle: más agentes
        tablaTransicion[28][PUNTOYCOMA] = 29; // Cierra lista de agentes
        
        // ========== SECCIÓN: topology (Estados 31-38) ==========
        
        // E29 -> E30: Después de agents, espera 'topology'
        tablaTransicion[29][TOPOLOGY] = 30;
        
        // E30 -> E31: 'topology' ... espera ':'
        tablaTransicion[30][DOS_PUNTOS] = 31;
        
        // E31 -> E32: 'topology:' ... espera ID (nombre de topología)
        tablaTransicion[31][IDENTIFICADOR] = 32;
        
        // E32 -> E33: 'topology: ID' ... espera '('
        tablaTransicion[32][PAREN_ABRE] = 33;
        
        // E33 -> E34 | E37: 'topology: ID(' ... espera ID (param) o ')' (sin params)
        tablaTransicion[33][IDENTIFICADOR] = 34; // Parámetro con nombre
        tablaTransicion[33][PAREN_CIERRA] = 37;  // Sin parámetros: complete()
        
        // E34 -> E35: 'topology: ID(param' ... espera ':'
        tablaTransicion[34][DOS_PUNTOS] = 35;
        
        // E35 -> E36: 'topology: ID(param:' ... espera valor
        tablaTransicion[35][NUMERO] = 36;
        tablaTransicion[35][DECIMAL] = 36;
        
        // E36 -> E33 | E37: 'topology: ID(param: val' ... espera ',' o ')'
        tablaTransicion[36][COMA] = 33;          // Bucle: más parámetros
        tablaTransicion[36][PAREN_CIERRA] = 37;  // Cierra parámetros
        
        // E37 -> E38: 'topology: ID(...)' ... espera ';'
        tablaTransicion[37][PUNTOYCOMA] = 38;
        
        // E38 -> E22: Después de topology, vuelve a E22 para link_props o cierre
        tablaTransicion[38][IDENTIFICADOR] = 999; // No se espera nada más aquí
        // En realidad, E38 debe permitir o 'link_props' o '}'
        
        // CORRECCIÓN: E38 no es necesario, E37 debe ir directamente a E22
        tablaTransicion[37][PUNTOYCOMA] = 22;    // Vuelve a E22
        
        // ========== SECCIÓN: link_props (OPCIONAL - Estados 40-46) ==========
        // E22 -> E40: Después de topology, puede venir 'link_props'
        tablaTransicion[22][LINK_PROPS] = 40;
        
        // E40 -> E41: 'link_props' ... espera ':'
        tablaTransicion[40][DOS_PUNTOS] = 41;
        
        // E41 -> E42: 'link_props:' ... espera '{'
        tablaTransicion[41][LLAVE_ABRE] = 42;
        
        // E42 -> E43 | E45: 'link_props: {' ... espera ID o '}'
        tablaTransicion[42][IDENTIFICADOR] = 43;
        tablaTransicion[42][LLAVE_CIERRA] = 45;  // link_props vacío
        
        // E43 -> E44: 'link_props: { ID' ... espera ':'
        tablaTransicion[43][DOS_PUNTOS] = 44;
        
        // E44 -> E48: 'link_props: { ID:' ... espera valor
        tablaTransicion[44][NUMERO] = 48;
        tablaTransicion[44][DECIMAL] = 48;
        tablaTransicion[44][CADENA] = 48;
        tablaTransicion[44][TRUE] = 48;
        tablaTransicion[44][FALSE] = 48;
        
        // E48 -> E42 | E45: 'link_props: { ID: val' ... espera ',' o '}'
        tablaTransicion[48][COMA] = 42;          // Bucle: más propiedades
        tablaTransicion[48][LLAVE_CIERRA] = 45;  // Cierra link_props
        
        // E45 -> E22: 'link_props: {...}' ... espera ';'
        tablaTransicion[45][PUNTOYCOMA] = 22;    // Vuelve a E22 para cerrar network
        
        // ========== CIERRE DEL BLOQUE NETWORK ==========
        // E22 -> E0: Cierra network con '}'
        tablaTransicion[22][LLAVE_CIERRA] = ESTADO_INICIAL;
        
        
        
        // ===================================================================
        // ========== TRANSICIONES PARA GLOBALS (Estados 60-65) ==========
        // Sintaxis: globals { (<id> : <valor>;)* }
        // ===================================================================

        // E107 (ESTADO_GLOBALS) -> E61 : 'globals' ... espera '{'
        tablaTransicion[ESTADO_GLOBALS][LLAVE_ABRE] = 61;

        // E61 -> E62 (ID) o E0 (vacío) : 'globals {' ... espera ID o '}'
        tablaTransicion[61][IDENTIFICADOR] = 62;
        tablaTransicion[61][LLAVE_CIERRA] = ESTADO_INICIAL; // Permite 'globals {}'

        // E62 -> E63 : 'globals { mi_var' ... espera ':'
        tablaTransicion[62][DOS_PUNTOS] = 63;



        // E63 (Esperando <valor>) -> E64 (Valor simple) o E65+ (Funciones)
        // ... 'globals { mi_var:' ... espera <valor>
        tablaTransicion[63][NUMERO]  = 64; // Valor asignado
        tablaTransicion[63][DECIMAL] = 64; // Valor asignado
        tablaTransicion[63][CADENA]  = 64; // Valor asignado
        tablaTransicion[63][TRUE]    = 64; // Valor asignado
        tablaTransicion[63][FALSE]   = 64; // Valor asignado

        // --- GANCHOS PARA FUTUROS INICIALIZADORES (Estados 65-79) ---
        tablaTransicion[63][RESTA]   = 65; // Iría a 'espera NUM/DEC'
        tablaTransicion[65][NUMERO]  = 64; // Valor asignado
        tablaTransicion[65][DECIMAL] = 64; // Valor asignado
    
        // E64 -> E61 (bucle) o E0 (fin) : '...: <valor>' ... espera ';' o '}'
        tablaTransicion[64][PUNTOYCOMA]  = 61; // Bucle a esperar siguiente ID
        tablaTransicion[64][LLAVE_CIERRA] = ESTADO_INICIAL; // Fin del bloque 'globals'
        tablaTransicion[64][COMA] = ERROR; // (Asegurarse que la coma NO sea válida)
        
        
        // ===================================================================
        // ========== TRANSICIONES PARA METRICS (Estados 75-99) ==========
        /*Sintaxis: metrics <id>{
            	collect [ (<funcion>(<parametro>) as <ID>)+ ] every <num> tick | at_end;
            } 
            */
        // ===================================================================
        
        
        // ENTRADA PRINCIPAL
        tablaTransicion[ESTADO_METRICS][IDENTIFICADOR] = 75;
        tablaTransicion[75][LLAVE_ABRE] = 76;
        
        // DENTRO DE METRICS
        tablaTransicion[76][COLLECT] = 77;
        tablaTransicion[76][LLAVE_CIERRA] = ESTADO_INICIAL;
        
        // ========== SECCIÓN: collect (77-99) ==========
        
        // E77 -> E78: 'collect' ... espera '['
        tablaTransicion[77][CORCHE_ABRE] = 78;
        
        // E78 → Puede ser función O acceso directo
        // Opción 1: Función de agregación
        tablaTransicion[78][COUNT] = 79;
        tablaTransicion[78][AVG] = 79;
        tablaTransicion[78][STDDEV] = 79;
        tablaTransicion[78][MIN] = 79;
        tablaTransicion[78][MAX] = 79;
        tablaTransicion[78][MEDIAN] = 79;
        tablaTransicion[78][VARIANCE] = 79;
        tablaTransicion[78][RANGE] = 79;
        tablaTransicion[78][AVG_LINK_PROP] = 79;
        
        // Opción 2: Acceso directo (NUEVO)
        //tablaTransicion[78][IDENTIFICADOR] = 92;  // agent.opinion, variable
        tablaTransicion[78][GLOBALS] = 92;        // globals.precio_mercado
        
        // ========== RUTA 1: Función de Agregación (79-84) ==========
        
        tablaTransicion[79][PAREN_ABRE] = 80;
        tablaTransicion[80][IDENTIFICADOR] = 81;
        tablaTransicion[80][CADENA] = 81;
        
        // Comparadores dentro del parámetro
        tablaTransicion[81][IGUAL] = 81;
        tablaTransicion[81][DISTINTO] = 81;
        tablaTransicion[81][MENOR] = 81;
        tablaTransicion[81][MAYOR] = 81;
        tablaTransicion[81][MENOR_IGUAL] = 81;
        tablaTransicion[81][MAYOR_IGUAL] = 81;
        tablaTransicion[81][TRUE] = 81;
        tablaTransicion[81][FALSE] = 81;
        tablaTransicion[81][NUMERO] = 81;
        tablaTransicion[81][DECIMAL] = 81;
        tablaTransicion[81][CADENA] = 81;
        tablaTransicion[81][PAREN_CIERRA] = 82;
        
        tablaTransicion[82][AS] = 83;
        tablaTransicion[83][IDENTIFICADOR] = 84;
        tablaTransicion[84][CORCHE_CIERRA] = 86; // Cierra lista
        
        // ========== RUTA 2: Acceso Directo (92-94) ==========
        
        // E92 → Después de IDENTIFICADOR o GLOBALS
        tablaTransicion[92][PUNTO] = 93;         // globals.precio (continúa)
        //tablaTransicion[92][AS] = 83;            // variable as alias (directo)
        
        // E93 → Después de PUNTO, espera propiedad
        tablaTransicion[93][IDENTIFICADOR] = 94;
        
        // E94 → Después de propiedad, puede venir:
        //tablaTransicion[94][PUNTO] = 93;         // Más puntos (ej: agent.vecino.opinion)
        tablaTransicion[94][AS] = 83;            // as alias
        tablaTransicion[94][CORCHE_CIERRA] = 86; // Cierra sin alias (opcional)
        
        // ========== CIERRE COMÚN (86-91) ==========
        
        tablaTransicion[86][EVERY] = 87;
        tablaTransicion[86][AT_END] = 90;
        
        tablaTransicion[87][NUMERO] = 88;
        tablaTransicion[88][TICK] = 89;
        tablaTransicion[89][PUNTOYCOMA] = 76;
        
        tablaTransicion[90][PUNTOYCOMA] = 76;
        
        tablaTransicion[76][LLAVE_CIERRA] = ESTADO_INICIAL;
        
        
        // ===================================================================
        // ========== TRANSICIONES PARA Experiment  (Estados 100-160) ==========
        /*Sintaxis:  experiment{
                apply_network: (<ID>);
                apply_dynamics: (<ID>);
                apply_metrics: (<ID>);
                duration: <num> ticks;
            }
            */
        // ===================================================================
        
        // ENTRADA PRINCIPAL
        tablaTransicion[ESTADO_EXPERIMENT][IDENTIFICADOR] = 140; // experiment ID
        tablaTransicion[140][LLAVE_ABRE] = 141;                  // experiment ID {
        
        // DENTRO DE EXPERIMENT
        tablaTransicion[141][APPLY_NETWORK] = 142;               // apply_network
        tablaTransicion[141][APPLY_DYNAMICS] = 145;              // apply_dynamics
        tablaTransicion[141][APPLY_METRICS] = 148;               // apply_metrics
        tablaTransicion[141][DURATION] = 151;                    // duration
        tablaTransicion[141][LLAVE_CIERRA] = ESTADO_INICIAL;     // }
        
        // ========== SECCIÓN: apply_network (142-144) ==========
        tablaTransicion[142][DOS_PUNTOS] = 143;                  // apply_network:
        tablaTransicion[143][IDENTIFICADOR] = 144;               // apply_network: ID
        tablaTransicion[144][PUNTOYCOMA] = 141;                  // apply_network: ID;
        
        // ========== SECCIÓN: apply_dynamics (145-147) ==========
        tablaTransicion[145][DOS_PUNTOS] = 146;                  // apply_dynamics:
        tablaTransicion[146][IDENTIFICADOR] = 147;               // apply_dynamics: ID
        tablaTransicion[147][PUNTOYCOMA] = 141;                  // apply_dynamics: ID;
        
        // ========== SECCIÓN: apply_metrics (148-150) ==========
        tablaTransicion[148][DOS_PUNTOS] = 149;                  // apply_metrics:
        tablaTransicion[149][IDENTIFICADOR] = 150;               // apply_metrics: ID
        tablaTransicion[150][PUNTOYCOMA] = 141;                  // apply_metrics: ID;
        
        // ========== SECCIÓN: duration (151-154) ==========
        tablaTransicion[151][DOS_PUNTOS] = 152;                  // duration:
        tablaTransicion[152][NUMERO] = 153;                      // duration: NUM
        tablaTransicion[153][TICKS] = 154;                       // duration: NUM ticks
        tablaTransicion[154][PUNTOYCOMA] = 141;                  // duration: NUM ticks;
        
        // CIERRE
        tablaTransicion[141][LLAVE_CIERRA] = ESTADO_INICIAL;     // }
        
        
        
        // ===================================================================
        // ========== TRANSICIONES PARA RUN  (Estados 161-199) ==========
        /*Sintaxis: 
            */
        // ===================================================================
        
        // ========== ENTRADA PRINCIPAL ==========
        tablaTransicion[ESTADO_RUN][LLAVE_ABRE] = 161;
        
        // ========== DENTRO DE RUN (E161) ==========
        tablaTransicion[161][TRIGGER] = 162;
        tablaTransicion[161][RESULT_SET] = 175;
        tablaTransicion[161][GRAPH] = 185;
        tablaTransicion[161][PRINT] = 192;
        tablaTransicion[161][EXPORT_CSV] = 196;
        tablaTransicion[161][LLAVE_CIERRA] = ESTADO_INICIAL;
        
        // ===================================================================
        // ========== SECCIÓN: trigger (162-174) ==========
        // Sintaxis: trigger <id> (at_tick(<num>)|on_start) { ... }
        
        // E162: trigger <ID>
        tablaTransicion[162][IDENTIFICADOR] = 163;
        
        // E163: trigger ID → (at_tick | on_start | on_condition)
        tablaTransicion[163][AT_TICK] = 164;
        tablaTransicion[163][ON_START] = 168;
        tablaTransicion[163][ON_CONDITION] = 164;  // Usa mismo camino que at_tick
        
        // E164: at_tick( | on_condition(
        tablaTransicion[164][PAREN_ABRE] = 165;
        
        // E165: at_tick(NUM | on_condition(expresion
        tablaTransicion[165][NUMERO] = 166;
        tablaTransicion[165][IDENTIFICADOR] = 165;  // Para on_condition: permite expresiones
        tablaTransicion[165][PUNTO] = 165;          // objeto.propiedad
        tablaTransicion[165][MENOR] = 165;
        tablaTransicion[165][MAYOR] = 165;
        tablaTransicion[165][MENOR_IGUAL] = 165;
        tablaTransicion[165][MAYOR_IGUAL] = 165;
        tablaTransicion[165][DECIMAL] = 165;
        
        // E166: at_tick(NUM)
        tablaTransicion[166][PAREN_CIERRA] = 168;
        
        // E168: trigger ... → {
        tablaTransicion[168][LLAVE_ABRE] = 169;
        
        // ========== DENTRO DE TRIGGER (E169-174) ==========
        
        // E169: Dentro de trigger
        tablaTransicion[169][STOP] = 170;
        //tablaTransicion[169][INJECT] = 171;
        tablaTransicion[169][PRINT] = 192;  // Reutiliza print global
        //cierra la llave de trigger
        tablaTransicion[169][LLAVE_CIERRA] = 182;//lleva aun punto y coma parecido al de resul_Set
        tablaTransicion[169][PUNTOYCOMA] = 169;
        
        // E170: stop;
        tablaTransicion[170][PUNTOYCOMA] = 169;
        

        
        // ===================================================================
        // ========== SECCIÓN: result_set (175-184) ==========
        // Sintaxis: result_set <id> = run_experiment(<id> [, triggers:[...]]);
        // ===================================================================
        
        // E175: result_set <ID>
        tablaTransicion[175][IDENTIFICADOR] = 176;
        
        // E176: result_set ID =
        tablaTransicion[176][ASIGN] = 177;
        
        // E177: result_set ID = run_experiment
        tablaTransicion[177][RUN_EXPERIMENT] = 178;
        
        // E178: result_set ID = run_experiment(
        tablaTransicion[178][PAREN_ABRE] = 179;
        
        // E179: run_experiment( → ID_experimento
        tablaTransicion[179][IDENTIFICADOR] = 180;
        
        // E180: run_experiment(ID → coma o cierre
        tablaTransicion[180][COMA] = 181;
        tablaTransicion[180][PAREN_CIERRA] = 182;
        
        // E181: run_experiment(ID, triggers:
        tablaTransicion[181][TRIGGER] = 181;  // Palabra "trigger" o "triggers"
        tablaTransicion[181][DOS_PUNTOS] = 183;
        
        // E183: triggers: [
        tablaTransicion[183][CORCHE_ABRE] = 184;
        
        // E184: triggers: [ → IDs
        tablaTransicion[184][IDENTIFICADOR] = 184;
        tablaTransicion[184][COMA] = 184;
        tablaTransicion[184][CORCHE_CIERRA] = 180;  // Vuelve a esperar cierre
        
        // E182: run_experiment(...);
        tablaTransicion[182][PUNTOYCOMA] = 161;
        
        // ===================================================================
        // ========== SECCIÓN: graph (185-191) ==========
        // Sintaxis: graph(series: [<id>.<prop>+], title: "...");
        // ===================================================================
        
        // E185: graph(
        tablaTransicion[185][PAREN_ABRE] = 186;
        
        // E186: graph( → series:
        tablaTransicion[186][SERIES] = 187;  // Espera "series"
        
        // E187: series:
        tablaTransicion[187][DOS_PUNTOS] = 188;
        
        // E188: series: [
        tablaTransicion[188][CORCHE_ABRE] = 189;
        
        // E189: series: [ → id.propiedad
        tablaTransicion[189][IDENTIFICADOR] = 190;
        
        // E190: series: [id → punto
        tablaTransicion[190][PUNTO] = 189;  // Bucle para más propiedades
        tablaTransicion[190][COMA] = 189;   // Más series
        tablaTransicion[190][CORCHE_CIERRA] = 186;  // Vuelve para más params
        
        // maneja title
        tablaTransicion[186][COMA] = 400;
        tablaTransicion[400][TITLE] = 401;
        tablaTransicion[401][DOS_PUNTOS] = 402;
        tablaTransicion[402][CADENA] = 186;
        tablaTransicion[186][PAREN_CIERRA] = 191;
        
        // E191: graph(...);
        tablaTransicion[191][PUNTOYCOMA] = 161;
        
        // ===================================================================
        // ========== SECCIÓN: print (192-194) ==========
        // Sintaxis: print("...");
        // ===================================================================
        
        // E192: print(
        tablaTransicion[192][PAREN_ABRE] = 193;
        
        // E193: print("...")
        tablaTransicion[193][CADENA] = 194;
        
        // E194: print("...")
        tablaTransicion[194][PAREN_CIERRA] = 195;
        
        // E195: print("...");
        tablaTransicion[195][PUNTOYCOMA] = 161;
        
        // ===================================================================
        // ========== SECCIÓN: export_csv (196-199) ==========
        // Sintaxis: export_csv(<id>, filename: "...");
        // ===================================================================
        
        // E196: export_csv(
        tablaTransicion[196][PAREN_ABRE] = 197;
        
        // E197: export_csv(id
        tablaTransicion[197][IDENTIFICADOR] = 198;
        
        // E198: export_csv(id, filename:
        tablaTransicion[198][COMA] = 403;  // Para filename
        tablaTransicion[403][FILENAME] = 404;  // Para filename
        tablaTransicion[404][DOS_PUNTOS] = 405;  // Para filename
        tablaTransicion[405][CADENA] = 198;  // Para filename
        tablaTransicion[198][PAREN_CIERRA] = 199;
        
        // E199: export_csv(...);
        tablaTransicion[199][PUNTOYCOMA] = 161;
        
        
        
        // ===================================================================
        // ========== TRANSICIONES PARA DYNAMICS (Estados 420-...) ==========
        /*Sintaxis: 
            */
        // ===================================================================
        tablaTransicion[ESTADO_DYNAMICS][IDENTIFICADOR] = 420;  // dynamics ID
        // E420: dynamics ID → {
        tablaTransicion[420][LLAVE_ABRE] = 421;
        // ========== DENTRO DE DYNAMICS (E421 - Hub Central) ==========
        tablaTransicion[421][ON_START] = 422;
        tablaTransicion[421][ON_INTERACTION] = 430;
        tablaTransicion[421][ON_AGENT_STEP] = 440;
        tablaTransicion[421][FOR_EACH_AGENT] = 450;
        tablaTransicion[421][ON_BROADCAST] = 460;
        tablaTransicion[421][ON_WORLD_STEP] = 470;
        tablaTransicion[421][LLAVE_CIERRA] = ESTADO_INICIAL;  // Cierra dynamics
        
        // ===================================================================
        // ========== SUB-BLOQUE: on_start (422-429) ==========
        // Sintaxis: on_start { (inject(...);)* }
        // ===================================================================
        
        // E422: on_start → {
        tablaTransicion[422][LLAVE_ABRE] = 423;
        
        // E423: Dentro de on_start (solo permite inject y cierre)
        //tablaTransicion[423][INJECT] = 423;  // Se maneja como subautómata
        tablaTransicion[423][PUNTOYCOMA] = 424;  // necesario para cerrar el subautómata y pasar al llave
        tablaTransicion[424][LLAVE_CIERRA] = 421;  // Vuelve al hub
        
        
        
        
        // ===================================================================
        // ========== SUB-BLOQUE: on_interaction (430-439) ==========
        // Sintaxis: on_interaction(A, B) [where (condicion)] { sentencias }
        // ===================================================================
        
        // E430: on_interaction → (
        tablaTransicion[430][PAREN_ABRE] = 431;
        
        // E431: on_interaction( → ID (primer parámetro A)
        tablaTransicion[431][IDENTIFICADOR] = 432;
        
        // E432: on_interaction(A → ,
        tablaTransicion[432][COMA] = 433;
        
        // E433: on_interaction(A, → ID (segundo parámetro B)
        tablaTransicion[433][IDENTIFICADOR] = 434;
        
        // E434: on_interaction(A, B → )
        tablaTransicion[434][PAREN_CIERRA] = 435;
        
        // E435: on_interaction(...) → where o {
        tablaTransicion[435][WHERE] = 436;
        tablaTransicion[435][LLAVE_ABRE] = 438;
        
        // ========== WHERE (436-437) - Manejo de condición ==========
        // E436: where → (
        tablaTransicion[436][PAREN_ABRE] = 437;
        
        // E437: where( → expresión booleana (acepta múltiples tokens)
        // Identificadores y propiedades
        tablaTransicion[437][IDENTIFICADOR] = 437;  // A, B, globals
        tablaTransicion[437][PUNTO] = 437;           // A.opinion
        
        // Comparadores
        tablaTransicion[437][IGUAL] = 437;           // ==
        tablaTransicion[437][DISTINTO] = 437;        // !=
        tablaTransicion[437][MENOR] = 437;           // <
        tablaTransicion[437][MAYOR] = 437;           // >
        tablaTransicion[437][MENOR_IGUAL] = 437;     // <=
        tablaTransicion[437][MAYOR_IGUAL] = 437;     // >=
        
        // Operadores lógicos
        tablaTransicion[437][OP_AND] = 437;          // &&
        tablaTransicion[437][OP_OR] = 437;           // ||
        
        // Valores literales
        tablaTransicion[437][TRUE] = 437;
        tablaTransicion[437][FALSE] = 437;
        tablaTransicion[437][NUMERO] = 437;
        tablaTransicion[437][DECIMAL] = 437;
        tablaTransicion[437][CADENA] = 437;
        
        // Paréntesis anidados en condición
        tablaTransicion[437][PAREN_ABRE] = 437;
        
        // Cierre de where
        tablaTransicion[437][PAREN_CIERRA] = 435;    // Vuelve a E435 para abrir {
        
        
        // ========== CUERPO DE on_interaction (438-439) ==========
        // E438: Dentro de on_interaction { ... }
        tablaTransicion[438][IDENTIFICADOR] = 439;   // Asignaciones, llamadas
        tablaTransicion[438][IF] = 480;              // Estructura if
        tablaTransicion[438][LLAVE_CIERRA] = 421;    // Cierra on_interaction, vuelve al hub
        
        // E439: Sentencias (asignaciones y expresiones)
        // Asignación
        tablaTransicion[439][ASIGN] = 439;           // =
        
        // Acceso a propiedades
        tablaTransicion[439][PUNTO] = 439;           // A.opinion
        
        // Operadores aritméticos
        tablaTransicion[439][SUMA] = 439;            // +
        tablaTransicion[439][RESTA] = 439;           // -
        tablaTransicion[439][MULT] = 439;            // *
        tablaTransicion[439][DIV] = 439;             // /
        
        // Paréntesis (para funciones o prioridad)
        tablaTransicion[439][PAREN_ABRE] = 439;
        tablaTransicion[439][PAREN_CIERRA] = 439;
        
        // Valores
        tablaTransicion[439][IDENTIFICADOR] = 439;   // Variables
        tablaTransicion[439][NUMERO] = 439;
        tablaTransicion[439][DECIMAL] = 439;
        
        // Fin de sentencia
        tablaTransicion[439][PUNTOYCOMA] = 438;      // Vuelve a esperar más sentencias
        
        // ===================================================================
        // ========== ESTRUCTURA IF/ELSE (480-489) - REUTILIZABLE ==========
        // ===================================================================
        
        // E480: if → (
        tablaTransicion[480][PAREN_ABRE] = 481;
        
        // E481: if( → condición (similar a where)
        tablaTransicion[481][IDENTIFICADOR] = 481;
        tablaTransicion[481][PUNTO] = 481;
        tablaTransicion[481][IGUAL] = 481;
        tablaTransicion[481][DISTINTO] = 481;
        tablaTransicion[481][MENOR] = 481;
        tablaTransicion[481][MAYOR] = 481;
        tablaTransicion[481][MENOR_IGUAL] = 481;
        tablaTransicion[481][MAYOR_IGUAL] = 481;
        tablaTransicion[481][OP_AND] = 481;
        tablaTransicion[481][OP_OR] = 481;
        tablaTransicion[481][TRUE] = 481;
        tablaTransicion[481][FALSE] = 481;
        tablaTransicion[481][NUMERO] = 481;
        tablaTransicion[481][DECIMAL] = 481;
        tablaTransicion[481][PAREN_ABRE] = 481;      // Paréntesis anidados
        tablaTransicion[481][PAREN_CIERRA] = 482;    // Cierra condición
        
        // E482: if(...) → {
        tablaTransicion[482][LLAVE_ABRE] = 483;
        
        // E483: Dentro de if { ... }
        tablaTransicion[483][IDENTIFICADOR] = 484;   // Sentencias
        tablaTransicion[483][IF] = 480;              // if anidado (recursivo)
        tablaTransicion[483][LLAVE_CIERRA] = 485;    // Cierra if
        
        // E484: Sentencias dentro de if (similar a E439)
        tablaTransicion[484][ASIGN] = 484;
        tablaTransicion[484][PUNTO] = 484;
        tablaTransicion[484][SUMA] = 484;
        tablaTransicion[484][RESTA] = 484;
        tablaTransicion[484][MULT] = 484;
        tablaTransicion[484][DIV] = 484;
        tablaTransicion[484][PAREN_ABRE] = 484;
        tablaTransicion[484][PAREN_CIERRA] = 484;
        tablaTransicion[484][IDENTIFICADOR] = 484;
        tablaTransicion[484][NUMERO] = 484;
        tablaTransicion[484][DECIMAL] = 484;
        tablaTransicion[484][PUNTOYCOMA] = 483;      // Fin sentencia, vuelve
        
        // E485: Después de if { ... } → else o continuar
        tablaTransicion[485][ELSE] = 486;
        tablaTransicion[485][IDENTIFICADOR] = 439;   // Más sentencias después del if
        tablaTransicion[485][IF] = 480;              // Otro if
        tablaTransicion[485][LLAVE_CIERRA] = 438;    // Cierra bloque padre
        tablaTransicion[485][PUNTOYCOMA] = 438;      // Por si viene ; suelto
        
        // E486: else → {
        tablaTransicion[486][LLAVE_ABRE] = 487;
        
        // E487: Dentro de else { ... }
        tablaTransicion[487][IDENTIFICADOR] = 488;
        tablaTransicion[487][IF] = 480;              // if dentro de else
        tablaTransicion[487][LLAVE_CIERRA] = 485;    // Cierra else, vuelve a E485
        
        // E488: Sentencias dentro de else
        tablaTransicion[488][ASIGN] = 488;
        tablaTransicion[488][PUNTO] = 488;
        tablaTransicion[488][SUMA] = 488;
        tablaTransicion[488][RESTA] = 488;
        tablaTransicion[488][MULT] = 488;
        tablaTransicion[488][DIV] = 488;
        tablaTransicion[488][PAREN_ABRE] = 488;
        tablaTransicion[488][PAREN_CIERRA] = 488;
        tablaTransicion[488][IDENTIFICADOR] = 488;
        tablaTransicion[488][NUMERO] = 488;
        tablaTransicion[488][DECIMAL] = 488;
        tablaTransicion[488][PUNTOYCOMA] = 487;

        
        
        // ====================================================================
        // ========== SUB-AUTÓMATAS COMPARTIDOS (Estados 200 - 399 ============
        // --- Sub-autómata para random(arg1, arg2) ---
        // (Inicia en ESTADO_RANDOM_INICIO = 200)
        tablaTransicion[200][PAREN_ABRE] = 201; // random(
        tablaTransicion[201][RESTA]   = 202; // random(-
        tablaTransicion[201][NUMERO]  = 203; // random(arg1
        tablaTransicion[201][DECIMAL] = 203;
        tablaTransicion[202][NUMERO]  = 203; // random(-arg1
        tablaTransicion[202][DECIMAL] = 203;
        tablaTransicion[203][COMA] = 204;    // random(arg1,
        tablaTransicion[204][RESTA]   = 205; // random(arg1, -
        tablaTransicion[204][NUMERO]  = 206; // random(arg1, arg2
        tablaTransicion[204][DECIMAL] = 206;
        tablaTransicion[205][NUMERO]  = 206; // random(arg1, -arg2
        tablaTransicion[205][DECIMAL] = 206;
        
        // ¡LA MAGIA! En lugar de ir a E9 o E64, vamos a "RETORNO"
        tablaTransicion[206][PAREN_CIERRA] = ESTADO_RETORNO; // -9

        // --- Sub-autómata para degree() ---
        // (Inicia en ESTADO_DEGREE_INICIO = 210)
        tablaTransicion[210][PAREN_ABRE] = 211; // degree(
        tablaTransicion[211][PAREN_CIERRA] = ESTADO_RETORNO; // degree() -> Retorno
        
        // --- Sub-autómata para normal(arg1, arg2) ---
        // (Inicia en ESTADO_NORMAL_INICIO = 220)
        tablaTransicion[220][PAREN_ABRE] = 221; // normal(
        tablaTransicion[221][RESTA]   = 222; // random(-
        tablaTransicion[221][NUMERO]  = 223; // random(arg1
        tablaTransicion[221][DECIMAL] = 223;
        tablaTransicion[222][NUMERO]  = 223; // random(-arg1
        tablaTransicion[222][DECIMAL] = 223;
        tablaTransicion[223][COMA] = 224;    // random(arg1,
        tablaTransicion[224][RESTA]   = 225; // random(arg1, -
        tablaTransicion[224][NUMERO]  = 226; // random(arg1, arg2
        tablaTransicion[224][DECIMAL] = 226;
        tablaTransicion[225][NUMERO]  = 226; // random(arg1, -arg2
        tablaTransicion[225][DECIMAL] = 226;
        tablaTransicion[226][PAREN_CIERRA] = ESTADO_RETORNO; // -9
        
        
        
        // --- Sub-autómata para INJECT(target: <cadena>, agent_type: <id>, props: {<prop>:<valor>}) ---
        // E240: inject( → espera 'target'
        tablaTransicion[239][PAREN_ABRE] = 240;
        tablaTransicion[240][TARGET] = 241;
        
        // E241: target: → espera cadena
        tablaTransicion[241][DOS_PUNTOS] = 242;
        
        // E242: target: "..." → captura cadena
        tablaTransicion[242][CADENA] = 243;
        
        // E243: Después de target, espera coma
        tablaTransicion[243][COMA] = 244;
        
        // E244: Puede venir agent_type o props
        tablaTransicion[244][AGENT_TYPE] = 245;
        tablaTransicion[244][PROPS] = 250;  // Salta directo a props si no hay agent_type
        
        // ========== Rama OPCIONAL: agent_type (245-249) ==========
        
        // E245: agent_type: → espera ID
        tablaTransicion[245][DOS_PUNTOS] = 246;
        
        // E246: agent_type: ID → captura identificador
        tablaTransicion[246][IDENTIFICADOR] = 247;
        
        // E247: Después de agent_type ID, espera coma
        tablaTransicion[247][COMA] = 248;
        
        // E248: Debe venir props obligatoriamente
        tablaTransicion[248][PROPS] = 250;
        
        // ========== Rama OBLIGATORIA: props (250-259) ==========
        
        // E250: props: → espera llave abierta
        tablaTransicion[250][DOS_PUNTOS] = 251;
        
        // E251: props: { → abre bloque
        tablaTransicion[251][LLAVE_ABRE] = 252;
        
        // E252: props: { → espera ID de propiedad
        tablaTransicion[252][IDENTIFICADOR] = 253;
        
        // E253: props: { ID → espera dos puntos
        tablaTransicion[253][DOS_PUNTOS] = 254;
        
        // E254: props: { ID: → espera valor
        tablaTransicion[254][NUMERO] = 255;
        tablaTransicion[254][DECIMAL] = 255;
        tablaTransicion[254][CADENA] = 255;
        tablaTransicion[254][TRUE] = 255;
        tablaTransicion[254][FALSE] = 255;
        tablaTransicion[254][RESTA] = 256;  // Para valores negativos
        
        // E256: props: { ID: - → espera numero/decimal
        tablaTransicion[256][NUMERO] = 255;
        tablaTransicion[256][DECIMAL] = 255;
        
        // E255: Después de valor, espera coma o llave cierra
        tablaTransicion[255][COMA] = 252;         // Más propiedades (bucle)
        tablaTransicion[255][LLAVE_CIERRA] = 257; // Cierra props
        
        // E257: props: {...} → espera cierre paréntesis
        tablaTransicion[257][PAREN_CIERRA] = ESTADO_RETORNO; // -9 (retorna)
        
        
        
        
    }
    
    void nextToken() {
        token = lex.getToken();
        
        // Asignar el valor solo si el token es de un tipo literal
        if (token == IDENTIFICADOR) {
            lastIdent = lex.variable;
        } 
        else if (token == NUMERO || token == DECIMAL) {
            lastNumber = lex.numero;
        } 
        else if (token == CADENA) {
            lastString = lex.cadena;
        }
    }

    // errorSintactico, errorSemantico (sin cambios)
    void errorSintactico(const string &msg) { 
         cerr << "ERROR SINTÁCTICO [Estado " << estadoActual << "]: " << msg
             << " (Token recibido: " << token << ", Lexema: '" << lex.variable << lex.numero << lex.cadena << "')" << endl;
        // Añadir más detalles si es necesario, como la línea/columna
            throw std::runtime_error("ERROR SINTÁCTICO: " + msg);

    }
    void errorSemantico(const string &msg) { 
         cerr << "ERROR SEMÁNTICO: " << msg << endl;
        throw std::runtime_error("ERROR SEMÁNTICO: " + msg);
    }

   void ejecutarAccion(int estadoPrevio, int tokenActual) {
    switch(estadoPrevio) {
        case ESTADO_INICIAL:
            if (tokenActual == GLOBALS) cout << "[ACCION] Iniciando bloque globals" << endl;
            else if (tokenActual == AGENT) cout << "[ACCION] Iniciando bloque agent" << endl;
            else if (tokenActual == NETWORK) cout << "[ACCION] Iniciando bloque network" << endl;
            else if (tokenActual == DYNAMICS) cout << "[ACCION] Iniciando bloque dynamics" << endl;
            else if (tokenActual == METRICS) cout << "[ACCION] Iniciando bloque metrics" << endl;
            else if (tokenActual == EXPERIMENT) cout << "[ACCION] Iniciando bloque experiment" << endl;
            else if (tokenActual == RUN) cout << "[ACCION] Iniciando bloque run" << endl;
            break;
        
        // ========================================
        // GLOBALS (Estados 60-79)
        // ========================================
        case ESTADO_GLOBALS:
            if (tokenActual == LLAVE_ABRE) cout << "[ACCION] Abriendo bloque globals {}" << endl;
            break;
        case 61:
            if (tokenActual == IDENTIFICADOR) cout << "[ACCION] Definiendo variable global: " << lastIdent << endl;
            else if (tokenActual == LLAVE_CIERRA) cout << "[ACCION] Cerrando bloque globals {} (vacío)" << endl;
            break;
        case 64:
            if (tokenActual == PUNTOYCOMA) cout << "[ACCION] Separador de variable global" << endl;
            else if (tokenActual == LLAVE_CIERRA) cout << "[ACCION] Cerrando bloque globals {}" << endl;
            break;
        
        // ========================================
        // AGENT (Estados 2-10)
        // ========================================
        case ESTADO_AGENT:
            if (tokenActual == IDENTIFICADOR) registrarDefinicion(lastIdent, "agent");
            break;
        case 2:
            if (tokenActual == LLAVE_ABRE) cout << "[ACCION] Abriendo bloque agent {}" << endl;
            break;
        case 3:
            if (tokenActual == PROPS) cout << "[ACCION] Iniciando sección props" << endl;
            else if (tokenActual == LLAVE_CIERRA) cout << "[ACCION] Cerrando bloque agent {} (vacío)" << endl;
            break;
        case 6:
            if (tokenActual == IDENTIFICADOR) cout << "[ACCION] Definiendo propiedad de agente: " << lastIdent << endl;
            else if (tokenActual == LLAVE_CIERRA) cout << "[ACCION] Cerrando bloque props {} (vacío)" << endl;
            break;
        case 9:
            if (tokenActual == COMA) cout << "[ACCION] Separador de propiedad" << endl;
            else if (tokenActual == LLAVE_CIERRA) cout << "[ACCION] Cerrando bloque props {}" << endl;
            break;
        case 7:
            if (tokenActual == LLAVE_CIERRA) cout << "[ACCION] Cerrando bloque agent {}" << endl;
            break;
        
        // ========================================
        // NETWORK (Estados 21-50)
        // ========================================
        case ESTADO_NETWORK:
            if (tokenActual == IDENTIFICADOR) registrarDefinicion(lastIdent, "network");
            break;
        case 21:
            if (tokenActual == LLAVE_ABRE) cout << "[ACCION] Abriendo bloque network {}" << endl;
            break;
        case 22:
            if (tokenActual == AGENTS) cout << "[ACCION] Iniciando sección agents" << endl;
            else if (tokenActual == LINK_PROPS) cout << "[ACCION] Iniciando sección link_props" << endl;
            else if (tokenActual == LLAVE_CIERRA) cout << "[ACCION] Cerrando bloque network {}" << endl;
            break;
        case 24:
            if (tokenActual == IDENTIFICADOR) {
                if (!existeComo(lastIdent, "agent")) {
                    errorSemantico("Network referencia agente no declarado: " + lastIdent);
                }
                cout << "[ACCION] Agente '" << lastIdent << "' validado para network" << endl;
            }
            break;
        case 26:
            if (tokenActual == NUMERO) cout << "[ACCION] Definiendo población de " << lastNumber << " agentes" << endl;
            break;
        case 28:
            if (tokenActual == COMA) cout << "[ACCION] Separador de población" << endl;
            else if (tokenActual == PUNTOYCOMA) cout << "[ACCION] Fin definición agents" << endl;
            break;
        case 29:
            if (tokenActual == TOPOLOGY) cout << "[ACCION] Iniciando sección topology" << endl;
            break;
        case 31:
            if (tokenActual == IDENTIFICADOR) cout << "[ACCION] Tipo de topología: " << lastIdent << endl;
            break;
        case 33:
            if (tokenActual == IDENTIFICADOR) cout << "[ACCION] Parámetro de topología: " << lastIdent << endl;
            else if (tokenActual == PAREN_CIERRA) cout << "[ACCION] Topología sin parámetros" << endl;
            break;
        case 36:
            if (tokenActual == COMA) cout << "[ACCION] Separador de parámetros" << endl;
            else if (tokenActual == PAREN_CIERRA) cout << "[ACCION] Cerrando params topology ()" << endl;
            break;
        case 37:
            if (tokenActual == PUNTOYCOMA) cout << "[ACCION] Fin definición topology" << endl;
            break;
        case 41:
            if (tokenActual == LLAVE_ABRE) cout << "[ACCION] Abriendo bloque link_props {}" << endl;
            break;
        case 42:
            if (tokenActual == IDENTIFICADOR) cout << "[ACCION] Definiendo prop de enlace: " << lastIdent << endl;
            else if (tokenActual == LLAVE_CIERRA) cout << "[ACCION] Cerrando bloque link_props {} (vacío)" << endl;
            break;
        case 48:
            if (tokenActual == COMA) cout << "[ACCION] Separador de prop de enlace" << endl;
            else if (tokenActual == LLAVE_CIERRA) cout << "[ACCION] Cerrando bloque link_props {}" << endl;
            break;
        case 45:
            if (tokenActual == PUNTOYCOMA) cout << "[ACCION] Fin definición link_props" << endl;
            break;
        
        // ========================================
        // METRICS (Estados 75-99)
        // Estados 80-81 compartidos con DYNAMICS
        // ========================================
        case ESTADO_METRICS:
            if (tokenActual == IDENTIFICADOR) registrarDefinicion(lastIdent, "metrics");
            break;
        case 75:
            if (tokenActual == LLAVE_ABRE) cout << "[ACCION] Abriendo bloque metrics {}" << endl;
            break;
        case 76:
            if (tokenActual == COLLECT) cout << "[ACCION] Iniciando sentencia collect" << endl;
            else if (tokenActual == LLAVE_CIERRA) cout << "[ACCION] Cerrando bloque metrics {}" << endl;
            break;
        case 77:
            if (tokenActual == CORCHE_ABRE) cout << "[ACCION] Abriendo lista de métricas [" << endl;
            break;
        case 78:
            if (tokenActual == COUNT || tokenActual == AVG || tokenActual == STDDEV ||
                tokenActual == MIN || tokenActual == MAX || tokenActual == MEDIAN) {
                cout << "[ACCION] Función de agregación: " << tokenActual << endl;
            }
            else if (tokenActual == IDENTIFICADOR) {
                cout << "[ACCION] Acceso a variable/propiedad: " << lastIdent << endl;
            }
            else if (tokenActual == GLOBALS) {
                cout << "[ACCION] Acceso a variable global" << endl;
            }
            break;
        
        // ESTADO 80: Compartido entre METRICS y DYNAMICS
        case 80:
            // Contexto METRICS: Parámetro de función
            if (tokenActual == IDENTIFICADOR) {
                cout << "[ACCION] Parámetro de función: " << lastIdent << endl;
            }
            else if (tokenActual == CADENA) {
                cout << "[ACCION] Parámetro literal: " << lastString << endl;
            }
            // Contexto DYNAMICS: Apertura de bloque
            else if (tokenActual == LLAVE_ABRE) {
                cout << "[ACCION] Abriendo bloque dynamics {}" << endl;
            }
            break;
        
        // ESTADO 81: Compartido entre METRICS y DYNAMICS
        case 81:
            // Contexto METRICS: Comparadores dentro de función
            if (tokenActual == IGUAL || tokenActual == DISTINTO ||
                tokenActual == MENOR || tokenActual == MAYOR) {
                cout << "[ACCION] Operador de comparación en condición" << endl;
            }
            else if (tokenActual == PAREN_CIERRA) {
                cout << "[ACCION] Fin parámetros de función" << endl;
            }
            // Contexto DYNAMICS: Bloques internos
            else if (tokenActual == ON_START) {
                cout << "[ACCION] Iniciando bloque on_start" << endl;
            }
            else if (tokenActual == ON_INTERACTION) {
                cout << "[ACCION] Iniciando bloque on_interaction" << endl;
            }
            else if (tokenActual == LLAVE_CIERRA) {
                cout << "[ACCION] Cerrando bloque dynamics {}" << endl;
            }
            break;
        
        case 82:
            if (tokenActual == AS) cout << "[ACCION] Palabra clave 'as'" << endl;
            break;
        case 83:
            if (tokenActual == IDENTIFICADOR) {
                cout << "[ACCION] Alias de métrica: " << lastIdent << endl;
            }
            break;
        case 84:
            if (tokenActual == COMA) cout << "[ACCION] Separador de métricas" << endl;
            else if (tokenActual == CORCHE_CIERRA) cout << "[ACCION] Cerrando lista de métricas ]" << endl;
            break;
        case 86:
            if (tokenActual == EVERY) cout << "[ACCION] Recolección periódica (every)" << endl;
            else if (tokenActual == AT_END) cout << "[ACCION] Recolección al final (at_end)" << endl;
            break;
        case 87:
            if (tokenActual == NUMERO) {
                cout << "[ACCION] Frecuencia: cada " << lastNumber << " ticks" << endl;
            }
            break;
        case 89:
            if (tokenActual == PUNTOYCOMA) cout << "[ACCION] Fin sentencia collect" << endl;
            break;
        case 90:
            if (tokenActual == PUNTOYCOMA) cout << "[ACCION] Fin sentencia collect (at_end)" << endl;
            break;
        
        // Acceso directo METRICS
        case 92:
            if (tokenActual == PUNTO) {
                cout << "[ACCION] Acceso a propiedad (punto)" << endl;
            }
            else if (tokenActual == AS) {
                cout << "[ACCION] Palabra clave 'as' (acceso directo)" << endl;
            }
            break;
        case 93:
            if (tokenActual == IDENTIFICADOR) {
                cout << "[ACCION] Propiedad: " << lastIdent << endl;
            }
            break;
        case 94:
            if (tokenActual == PUNTO) {
                cout << "[ACCION] Acceso anidado (más puntos)" << endl;
            }
            else if (tokenActual == AS) {
                cout << "[ACCION] Palabra clave 'as'" << endl;
            }
            else if (tokenActual == CORCHE_CIERRA) {
                cout << "[ACCION] Cerrando lista (sin alias)" << endl;
            }
            break;
        
        // ========================================
        // DYNAMICS - on_interaction
        // ========================================
        case 430:
            if (tokenActual == PAREN_ABRE) cout << "[ACCION] Iniciando parámetros on_interaction" << endl;
            break;
        
        case 431:
            if (tokenActual == IDENTIFICADOR) {
                cout << "[ACCION] Parámetro A: " << lastIdent << endl;
            }
            break;
        
        case 433:
            if (tokenActual == IDENTIFICADOR) {
                cout << "[ACCION] Parámetro B: " << lastIdent << endl;
            }
            break;
        
        
        // ========================================
        // EXPERIMENT (Estados 140-159)
        // ========================================
        case ESTADO_EXPERIMENT: // 105
            if (tokenActual == IDENTIFICADOR) registrarDefinicion(lastIdent, "experiment");
            break;
        
        case 140: // experiment ID {
            if (tokenActual == LLAVE_ABRE) cout << "[ACCION] Abriendo bloque experiment {}" << endl;
            break;
        
        case 141: // Dentro de experiment
            if (tokenActual == APPLY_NETWORK) cout << "[ACCION] Iniciando apply_network" << endl;
            else if (tokenActual == APPLY_DYNAMICS) cout << "[ACCION] Iniciando apply_dynamics" << endl;
            else if (tokenActual == APPLY_METRICS) cout << "[ACCION] Iniciando apply_metrics" << endl;
            else if (tokenActual == DURATION) cout << "[ACCION] Iniciando duration" << endl;
            else if (tokenActual == LLAVE_CIERRA) cout << "[ACCION] Cerrando bloque experiment {}" << endl;
            break;
        
        // apply_network
        case 143:
            if (tokenActual == IDENTIFICADOR) {
                cout << "[ACCION] Referenciando network: " << lastIdent << endl;
            }
            break;
        
        case 144:
            if (tokenActual == PUNTOYCOMA) cout << "[ACCION] Fin apply_network" << endl;
            break;
        
        // apply_dynamics
        case 146:
            if (tokenActual == IDENTIFICADOR) {
                cout << "[ACCION] Referenciando dynamics: " << lastIdent << endl;
            }
            break;
        
        case 147:
            if (tokenActual == PUNTOYCOMA) cout << "[ACCION] Fin apply_dynamics" << endl;
            break;
        
        // apply_metrics
        case 149:
            if (tokenActual == IDENTIFICADOR) {
                cout << "[ACCION] Referenciando metrics: " << lastIdent << endl;
            }
            break;
        
        case 150:
            if (tokenActual == PUNTOYCOMA) cout << "[ACCION] Fin apply_metrics" << endl;
            break;
        
        // duration
        case 152:
            if (tokenActual == NUMERO) {
                cout << "[ACCION] Duración de simulación: " << lastNumber << " ticks" << endl;
            }
            break;
        
        case 153:
            if (tokenActual == TICKS) cout << "[ACCION] Palabra clave 'ticks'" << endl;
            break;
        
        case 154:
            if (tokenActual == PUNTOYCOMA) cout << "[ACCION] Fin duration" << endl;
            break;
        
        // ========================================
        // RUN (Estados 161-199)
        // ========================================
        case ESTADO_RUN:
            if (tokenActual == LLAVE_ABRE) cout << "[ACCION] Abriendo bloque run {}" << endl;
            break;
        
        case 161:
            if (tokenActual == TRIGGER) cout << "[ACCION] Iniciando trigger" << endl;
            else if (tokenActual == RESULT_SET) cout << "[ACCION] Iniciando result_set" << endl;
            else if (tokenActual == GRAPH) cout << "[ACCION] Iniciando graph" << endl;
            else if (tokenActual == PRINT) cout << "[ACCION] Iniciando print" << endl;
            else if (tokenActual == LLAVE_CIERRA) cout << "[ACCION] Cerrando bloque run {}" << endl;
            break;
        
        // trigger
        case 162:
            if (tokenActual == IDENTIFICADOR) cout << "[ACCION] Trigger ID: " << lastIdent << endl;
            break;
        
        case 163:
            if (tokenActual == AT_TICK) cout << "[ACCION] Condición: at_tick" << endl;
            else if (tokenActual == ON_START) cout << "[ACCION] Condición: on_start" << endl;
            else if (tokenActual == ON_CONDITION) cout << "[ACCION] Condición: on_condition" << endl;
            break;
        
        case 169:
            if (tokenActual == STOP) cout << "[ACCION] Sentencia: stop" << endl;
            // ¡NO hay caso para INJECT aquí!
            else if (tokenActual == PRINT) cout << "[ACCION] Sentencia: print" << endl;
            else if (tokenActual == LLAVE_CIERRA) cout << "[ACCION] Cerrando trigger {}" << endl;
            break;
        
        case 170:
            if (tokenActual == PUNTOYCOMA) cout << "[ACCION] Fin stop;" << endl;
            break;
        
        // ========================================
        // result_set (Estados 175-184)
        // ========================================
        case 175:
            if (tokenActual == IDENTIFICADOR) {
                cout << "[ACCION] Variable result_set: " << lastIdent << endl;
            }
            break;
        
        case 177:
            if (tokenActual == RUN_EXPERIMENT) {
                cout << "[ACCION] Llamada a run_experiment" << endl;
            }
            break;
        
        case 179:
            if (tokenActual == IDENTIFICADOR) {
                cout << "[ACCION] ID experimento: " << lastIdent << endl;
            }
            break;
        
        case 182:
            if (tokenActual == PUNTOYCOMA) {
                cout << "[ACCION] Fin result_set" << endl;
            }
            break;
            
        // ========================================
        // graph (Estados 185-191)
        // ========================================
        case 185:
            if (tokenActual == PAREN_ABRE) {
                cout << "[ACCION] Iniciando parámetros graph()" << endl;
            }
            break;
        
        case 189:
            if (tokenActual == IDENTIFICADOR) {
                cout << "[ACCION] Serie: " << lastIdent << endl;
            }
            break;
        
        case 190:
            if (tokenActual == PUNTO) {
                cout << "[ACCION] Acceso a propiedad (serie.prop)" << endl;
            }
            break;
        
        case 191:
            if (tokenActual == PUNTOYCOMA) {
                cout << "[ACCION] Fin graph()" << endl;
            }
            break;
            
        // ========================================
        // print (Estados 192-195)
        // ========================================
        case 192:
            if (tokenActual == PAREN_ABRE) {
                cout << "[ACCION] Iniciando print()" << endl;
            }
            break;
        
        case 193:
            if (tokenActual == CADENA) {
                cout << "[ACCION] Mensaje: " << lastString << endl;
            }
            break;
        
        case 195:
            if (tokenActual == PUNTOYCOMA) {
                cout << "[ACCION] Fin print()" << endl;
            }
            break;
        
        // ========================================
        // SUBAUTÓMATA: inject (Estados 240-260)
        // ========================================
        case 240:
            if (tokenActual == TARGET) cout << "[ACCION] Parámetro: target" << endl;
            break;
        
        case 242:
            if (tokenActual == CADENA) {
                cout << "[ACCION] Target value: " << lastString << endl;
            }
            break;
        
        case 244:
            if (tokenActual == AGENT_TYPE) cout << "[ACCION] Parámetro opcional: agent_type" << endl;
            else if (tokenActual == PROPS) cout << "[ACCION] Parámetro: props" << endl;
            break;
        
        case 246:
            if (tokenActual == IDENTIFICADOR) {
                cout << "[ACCION] Agent type: " << lastIdent << endl;
            }
            break;
        
        case 252:
            if (tokenActual == IDENTIFICADOR) {
                cout << "[ACCION] Propiedad inject: " << lastIdent << endl;
            }
            break;
        
        case 254:
            if (tokenActual == NUMERO || tokenActual == DECIMAL) {
                cout << "[ACCION] Valor prop: " << lastNumber << endl;
            }
            else if (tokenActual == CADENA) {
                cout << "[ACCION] Valor prop: " << lastString << endl;
            }
            else if (tokenActual == TRUE || tokenActual == FALSE) {
                cout << "[ACCION] Valor prop: " << (tokenActual == TRUE ? "true" : "false") << endl;
            }
            break;
        
        case 257:
            if (tokenActual == PAREN_CIERRA) cout << "[ACCION] Fin inject()" << endl;
            break;
    }
}

    // ========================================
    // FUNCIÓN PARA DETERMINAR CONTEXTO
    // ========================================
    ContextoBloque determinarContexto(int estado) {
        // ========== GLOBALS ==========
        if (estado == ESTADO_GLOBALS) { // 107
            return ContextoBloque("GLOBALS", ESTADO_INICIAL);
        }
        
        // ========== AGENT ==========
        else if (estado == 2) {
            return ContextoBloque("AGENT", ESTADO_INICIAL, lastIdent);
        }
        else if (estado == 5) {
            return ContextoBloque("PROPS", 7); // props dentro de agent
        }
        
        // ========== NETWORK ==========
        else if (estado == 21) { // CORREGIDO (antes era 12)
            return ContextoBloque("NETWORK", ESTADO_INICIAL, lastIdent);
        }
        else if (estado == 41) { // CORREGIDO (antes era 34)
            return ContextoBloque("LINK_PROPS", 22); // CORREGIDO (antes era 20)
            // link_props debe volver a E22, no a E20
        }
        
        // ========== DYNAMICS ==========
        else if (estado == 420) {
            return ContextoBloque("DYNAMICS", ESTADO_INICIAL, lastIdent);
        }
        
        else if (estado == 422) {
            return ContextoBloque("DYNAMICS_On_start", 421);
        }
        
        else if (estado == 435) {
        return ContextoBloque("ON_INTERACTION", 421);
        }
        
        else if (estado == 482) {
        return ContextoBloque("ON_INTERACTION_if", 438);
        }
        else if (estado == 483) {
        return ContextoBloque("ON_INTERACTION_if", 438);
        }
        
        
        // Aquí agregarías sub-bloques de dynamics:
        // else if (estado == XX) return ContextoBloque("ON_START", YY);
        // else if (estado == XX) return ContextoBloque("ON_INTERACTION", YY);
        
        // ========== METRICS ==========
        else if (estado == 75) { // CORREGIDO (antes era 100)
            return ContextoBloque("METRICS", ESTADO_INICIAL, lastIdent);
        }
        
        // ========== EXPERIMENT ==========
        else if (estado == 140) { // CORREGIDO
            return ContextoBloque("EXPERIMENT", ESTADO_INICIAL, lastIdent);
        }
        // ========== RUN ==========
        else if (estado == 106) {
            return ContextoBloque("RUN", ESTADO_INICIAL);
        }
        else if (estado == 168) {//estado antes de abrir las llaves
            return ContextoBloque("inject_trigger", 161);//estado despues de cerrar las llaves
        }
        else if (estado == 251) {//estado antes de abrir las llaves
            return ContextoBloque("inject_props", 257);//estado despues de cerrar las llaves
        }
        
        // ========== DESCONOCIDO ==========
        else {
            return ContextoBloque("DESCONOCIDO", -1);
        }
    }

public:
    AnalizadorSintactico(AnalizadorLexico &l): lex(l) {
        token = -1;
        lastIdent = "";
        lastNumber = "";
        lastString = "";
        estadoActual = ESTADO_INICIAL;
        inicializarTablaTransicion();
    }

    // registrarDefinicion, existeComo SIN CAMBIOS
    void registrarDefinicion(const string &nombre, const string &tipo) {
        Atributos a;
        if (ts_usuario.Buscar(nombre, a)) {
            errorSemantico("Identificador ya declarado: " + nombre);
        }
        int tok = IDENTIFICADOR;
        ts_usuario.Insertar(nombre, tok, tipo, "-", "declarado");
         cout << "[ACCION] " << tipo << " '" << nombre << "' registrado" << endl;
    }
    
    bool existeComo(const string &nombre, const string &tipo) { 
        Atributos a;
        return ts_usuario.BuscarPorTipo(nombre, tipo, a);
    }

    // Programa es EL BUCLE PRINCIPAL
    void Programa() {
        nextToken();
        cout << "[INFO] Iniciando análisis con pila..." << endl;
        cout << "[INFO] Iniciando análisis con control de llaves..." << endl;
        
        while (token != FIN) {
            // ========================================
            // FASE 1: DETECTAR APERTURA DE BLOQUES {}
            // ========================================
            if (token == LLAVE_ABRE) {
                ContextoBloque contexto = determinarContexto(estadoActual);
               
                if (contexto.estadoRetorno != -1) {
                    pilaContextos.push(contexto);
                    
                    cout << "[PILA CONTEXTOS PUSH] Abriendo '" << contexto.tipoBloque;
                    if (!contexto.nombreBloque.empty()) {
                        cout << " " << contexto.nombreBloque;
                    }
                    cout << "' -> Estado retorno: E" << contexto.estadoRetorno
                         << " (Nivel bloques: " << pilaContextos.size() << ")" << endl;
                }
            }
            
            // ========================================
            // FASE 2: DETECTAR CIERRE DE BLOQUES {}
            // ========================================
            else if (token == LLAVE_CIERRA) {
                cout<<estadoActual<<endl;
                
                if (!pilaContextos.empty()) {
                    ContextoBloque contexto = pilaContextos.top();
                    pilaContextos.pop();
                    
                    cout << "[PILA CONTEXTOS POP] Cerrando '" << contexto.tipoBloque;
                    if (!contexto.nombreBloque.empty()) {
                        cout << " " << contexto.nombreBloque;
                    }
                    cout << "' (Nivel bloques: " << pilaContextos.size() << ")" << endl;
                    
                    int siguienteEstado = tablaTransicion[estadoActual][token];
                    cout<<siguienteEstado<<endl;

                    
                    if (siguienteEstado == ERROR) {
                        errorSintactico("Cierre '}' inesperado en '" + 
                                      contexto.tipoBloque + "'");
                    }
                    
                    ejecutarAccion(estadoActual, token);
                    estadoActual = contexto.estadoRetorno;
                    nextToken();
                    continue;
                }
                // Si la pila está vacía, deja que la tabla maneje el error
            }
        
            //===================
            //Pila de subautomatas
            //===================
            bool esLlamadaSubautomata = false;
            int estadoSubautomata = -1;
            int estadoRetorno = -1;
            
    
            if (token == RANDOM) {
                estadoSubautomata = ESTADO_RANDOM_INICIO;
                esLlamadaSubautomata = true;
                cout<<"AQUI_RANDOM"<<estadoActual<<endl;
                // Determinar estado de retorno según contexto
                if (estadoActual == 10) estadoRetorno = 9;        // Agent props
                else if (estadoActual == 63) estadoRetorno = 64;  // Globals
                //else if (estadoActual == 260) estadoRetorno = 261;  // Dynamics_on_interact(,)
                else if (estadoActual == 481) estadoRetorno = 481;  // Dynamics_on_interact(,)

                else {
                    errorSintactico("random() no permitido aquí");
                }
            }
            else if (token == DEGREE) {
                estadoSubautomata = ESTADO_DEGREE_INICIO;
                esLlamadaSubautomata = true;
                
                if (estadoActual == 10) estadoRetorno = 9;
                else if (estadoActual == 63) estadoRetorno = 64;
                else {
                    errorSintactico("degree() no permitido aquí");
                }
            }
            
            
            else if (token == NORMAL) {
                estadoSubautomata = ESTADO_NORMAL_INICIO;
                esLlamadaSubautomata = true;
                
                if (estadoActual == 10) estadoRetorno = 9;
                else if (estadoActual == 63) estadoRetorno = 64;
                else {
                    errorSintactico("normal() no permitido aquí");
                }
            }
            
             // ¡NUEVO! INJECT
            else if (token == INJECT) {
                estadoSubautomata = ESTADO_INJECT_INICIO; // 240
                esLlamadaSubautomata = true;
                
                // inject() solo se permite dentro de triggers (E169)
                if (estadoActual == 169) {
                    estadoRetorno = 169; // Vuelve dentro del trigger
                }
                
                 else if (estadoActual == 423) {  // ¡NUEVO!
                    estadoRetorno = 423; // Vuelve dentro de on_start
                }
                else {
                    errorSintactico("inject() solo permitido dentro de trigger (estado E" + 
                                  to_string(estadoActual) + ")");
                }
            }
            
            
        
            // Si es llamada a subautómata, hacer PUSH y cambiar estado
            if (esLlamadaSubautomata) {
                pila.push(estadoRetorno);
                cout << "[PILA PUSH] Estado retorno: E" << estadoRetorno 
                     << " (Pila size: " << pila.size() << ")" << endl;
                estadoActual = estadoSubautomata;
                nextToken();
                continue; // ¡Ahora sí usar continue!
            }
        
            // --- FASE 2: Consultar tabla de transiciones ---
            if (token < 0 || token >= 1000 || estadoActual < 0 || estadoActual >= 1000) {
                errorSintactico("Token o Estado fuera de rango");
            }
        
            int siguienteEstado = tablaTransicion[estadoActual][token];
            
            // --- FASE 3: Manejar retornos ---
            if (siguienteEstado == ESTADO_RETORNO) {
                if (pila.empty()) {
                    errorSintactico("Retorno inesperado, pila vacía");
                }
                siguienteEstado = pila.top();
                pila.pop();
                cout << "[PILA POP] Retornando a E" << siguienteEstado 
                     << " (Pila size: " << pila.size() << ")" << endl;
            }
            
            // ---  Validar transición ---
           if (siguienteEstado == ERROR) {
                string contexto = pilaContextos.empty() ? 
                                 "raíz" : pilaContextos.top().tipoBloque;
                errorSintactico("Transición no válida en contexto '" + contexto + 
                              "' desde E" + to_string(estadoActual) + 
                              " con token " + to_string(token));
            }
            
             cout << "[TRANSICION] E" << estadoActual << " -> E" 
                 << siguienteEstado << " (token: " << token << ")" << endl;
            
            ejecutarAccion(estadoActual, token);
            estadoActual = siguienteEstado;
            nextToken();
            
        }
        
        cout << "========================================" << endl;
        cout << "[INFO] Verificando estado final..." << endl;
        
        bool hayErrores = false;
        
        if (!pila.empty()) {
            cerr << "[ERROR] Pila de funciones no vacía. " 
                 << pila.size() << " función(es) sin cerrar" << endl;
            hayErrores = true;
        }
        
        if (!pilaContextos.empty()) {
            cerr << "[ERROR] Pila de contextos no vacía. " 
                 << pilaContextos.size() << " bloque(s) sin cerrar:" << endl;
            while (!pilaContextos.empty()) {
                cerr << "  - " << pilaContextos.top().tipoBloque << endl;
                pilaContextos.pop();
            }
            hayErrores = true;
        }
        
        if (hayErrores) {
            cerr << "========================================" << endl;
            exit(1);
        }
        
        cout << "[OK] Ambas pilas vacías" << endl;
        cout << "[OK] Análisis finalizado correctamente" << endl;
        cout << "========================================" << endl;
    }

    // MostrarTablaSimbolos SIN CAMBIO
    void MostrarTablaSimbolos() { 
        cout << "---- Tabla de símbolos (definiciones de usuario) ----" << endl;
        ts_usuario.Mostrar();
    }

    // Mostrar tabla de transiciones (ACTUALIZADO para incluir más tokens/estados relevantes)
    void MostrarTablaTransiciones() {
        cout << "---- Tabla de Transiciones (Parcial - Definidas en Matriz) ----" << endl;
        cout << "Estado\\Token\t";
        // Lista ampliada para mostrar más contexto
        int tokens[] = {GLOBALS, AGENT, NETWORK, DYNAMICS, METRICS, EXPERIMENT, TRIGGER, RUN,
                        IDENTIFICADOR, LLAVE_ABRE, LLAVE_CIERRA, PROPS, DOS_PUNTOS, NUMERO, DECIMAL, CADENA, TRUE, FALSE,
                        RANDOM, NORMAL, DEGREE, COMA, PUNTOYCOMA, AGENTS, CORCHE_ABRE, CORCHE_CIERRA,
                        TOPOLOGY, LINK_PROPS, PAREN_ABRE, PAREN_CIERRA,
                        COLLECT, AS, EVERY, AT_END, AT_TICK, APPLY_NETWORK, DURATION, RESULT_SET,
                        ON_START, ON_INTERACTION, ON_AGENT_STEP, ON_WORLD_STEP, FOR_EACH_AGENT, WHERE, STOP,
                        FIN};
        string tokenNames[] = {"GLOB", "AGENT", "NET", "DYN", "MET", "EXP", "TRIG", "RUN",
                               "ID", "{", "}", "PROPS", ":", "NUM", "DEC", "STR", "TRU", "FAL",
                               "RND", "NOR", "DEG", ",", ";", "AGENTS", "[", "]",
                               "TOP", "LNK_P", "(", ")",
                               "COLL", "AS", "EVRY", "END", "AT_T", "AP_N", "DUR", "R_SET",
                               "ON_S", "ON_I", "ON_AS", "ON_WS", "FOR", "WHER", "STOP",
                               "FIN"};
        const int numTokensVisibles = sizeof(tokens) / sizeof(tokens[0]);

        for(int i = 0; i < numTokensVisibles; i++) {
            if (strlen(tokenNames[i].c_str()) < 4) cout << tokenNames[i] << "\t\t";
            else cout << tokenNames[i] << "\t";
        }
        cout << endl;

        // Mostrar un rango más amplio de estados
        for(int estado = 0; estado <= 130; estado++) { // Ampliado rango
             bool tieneTransicionVisible = false;
             for(int i = 0; i < numTokensVisibles; i++) {
                 if (tokens[i] >= 0 && tokens[i] < 410) {
                     if(tablaTransicion[estado][tokens[i]] != ERROR) {
                         tieneTransicionVisible = true;
                         break;
                     }
                 }
             }
             // Mostrar solo si tiene transiciones visibles O es un estado inicial/final importante
             if (!tieneTransicionVisible && estado != ESTADO_INICIAL && estado != ESTADO_FINAL && estado != ESTADO_AGENT && estado != ESTADO_NETWORK && estado != ESTADO_DYNAMICS && estado != ESTADO_METRICS && estado != ESTADO_EXPERIMENT && estado != ESTADO_RUN && estado != ESTADO_GLOBALS) continue;

            cout << "E" << estado << "\t\t";
            for(int i = 0; i < numTokensVisibles; i++) {
                 if (tokens[i] >= 0 && tokens[i] < 410) {
                    int trans = tablaTransicion[estado][tokens[i]];
                    if(trans != ERROR) {
                        cout << "E" << trans ;
                    } else {
                        cout << "-";
                    }
                 } else {
                     cout << "?"; // Token fuera de rango
                 }
                 // Ajustar tabulación
                 if (strlen(tokenNames[i].c_str()) < 4) cout << "\t\t";
                 else cout << "\t";
            }
            cout << endl;
        }
    }
    
};


string obtenerCodigoDeArgumentoOArchivo(int argc, char* argv[]) {
    if (argc < 2) {
        return ""; // No se pasó nada
    }
    string argumento = argv[1];

    // Si el argumento es un archivo existente, lo lee; si no, lo toma como código directo
    ifstream archivo(argumento);
    if (archivo.good()) {
        std::ostringstream buffer;
        buffer << archivo.rdbuf();
        return buffer.str();
    }
    // Si no es archivo, regresamos el argumento textual como fuente (código en línea)
    return argumento;
}

string procesarCodigoCompleto(const string& codigo) {
    ostringstream salida;
    
    // Redirigir cout a nuestro buffer
    streambuf* coutOriginal = cout.rdbuf();
    cout.rdbuf(salida.rdbuf());

    try {
            cerr << "--- [DEBUG] Iniciando análisis ---" << endl;
            AnalizadorLexico lex;
            lex.cargarCodigo(codigo);
            
            cerr << "--- [DEBUG] Léxico OK. Creando Sintáctico..." << endl;
            AnalizadorSintactico sint(lex);
            
            cerr << "--- [DEBUG] Ejecutando sint.Programa()... ---" << endl;
            sint.Programa(); // <--- ¿El error ocurre ANTES o DESPUÉS de esta línea?
            
            cerr << "--- [DEBUG] Ejecutando sint.MostrarTablaSimbolos()... ---" << endl;
            sint.MostrarTablaSimbolos(); // <--- ¿O falla aquí?
            
            cerr << "--- [DEBUG] Análisis completado sin error ---" << endl;
            
            cout.rdbuf(coutOriginal);
            return salida.str();

        } catch (const std::exception& e) { // Es mejor capturar excepciones específicas
            cerr << "¡Excepción de C++ capturada!: " << e.what() << endl;
            cout.rdbuf(coutOriginal);
            return "ERROR: Excepción durante análisis: " + string(e.what());
        } catch (...) {
            cerr << "¡Excepción de C++ desconocida capturada!" << endl;
            cout.rdbuf(coutOriginal);
            return "ERROR: Excepción desconocida durante análisis";
        }
}

extern "C" {
  
  EMSCRIPTEN_KEEPALIVE
  const char* compilarCodigo(const char* input) {
    try {
      // 1. Llama a tu función que ya procesa todo
      bufferGlobal = procesarCodigoCompleto(std::string(input));
      
      // 2. Retorna un puntero C al buffer global.
      // (Es seguro porque 'bufferGlobal' es estático y no se destruye)
      return bufferGlobal.c_str();
      
    } catch (const std::exception& e) {
      // Captura cualquier error de C++
      bufferGlobal = "Error interno del compilador C++: ";
      bufferGlobal += e.what();
      return bufferGlobal.c_str();
      
    } catch (...) {
      // Captura cualquier otra excepción desconocida
      bufferGlobal = "Error interno desconocido del compilador C++.";
      return bufferGlobal.c_str();
    }
  }
  
}
