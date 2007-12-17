/* Nota: En las librerias generadas desde Visual Studio, y luego utilizadas con MinGW,
 *       los ficheros ".h" que vayan a ser inclu�dos, deben eliminar las "keywords"
 *       que son "calling convention" del estilo __stdcall, o generan error del compilador.
 *       Tambi�n los tipos BOOL deben ser sustituidos por bool en min�sculas.
 */
#ifdef __cplusplus
extern "C" {
#endif

enum HookType { Keyboard, Mouse };
extern bool InstallHook (HookType type);

#ifdef __cplusplus
}
#endif
