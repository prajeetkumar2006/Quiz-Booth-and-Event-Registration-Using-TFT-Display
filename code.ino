#include <MCUFRIEND_kbv.h>
#include <Adafruit_GFX.h>
#include <TouchScreen.h>

MCUFRIEND_kbv tft;

// -------- TOUCH CALIBRATION --------
#define YP A3
#define XM A2
#define YM 9
#define XP 8
#define MINPRESSURE 200
#define MAXPRESSURE 1000
#define TS_MINX 120
#define TS_MAXX 900
#define TS_MINY 70
#define TS_MAXY 920

TouchScreen ts(XP, YP, XM, YM, 300);

// -------- COLORS --------
#define BLACK   0x0000
#define WHITE   0xFFFF
#define GREEN   0x07E0
#define RED     0xF800
#define BLUE    0x001F
#define YELLOW  0xFFE0
#define CYAN    0x07FF

// -------- DATA ARRAYS --------
String names[5]     = {"Thanos","Knull","DrDoom","Galactus","Mephisto"};
String usernames[5] = {"660101","660102","660103","660104","660105"};
String passwords[5] = {"1234","2345","3456","4567","5678"};
int userScores[5]   = {0, 0, 0, 0, 0}; 
bool hasAttendedQuiz[5] = {false, false, false, false, false};

// Stores Name and Username for registration
struct RegEntry {
  String name;
  String id;
};
RegEntry registeredUsers[20];
int regCount = 0;

String events[15] = {"Crack N Bing", "Decode & Conquer", "", "", "", "", "", "", "", "", "", "", "", "", ""};
int eventCount = 2; 
int selectedEventIdx = -1;
int eventPage = 0; 

String USERS_WINNERS[15][3] = {
  {"Mr Sinster", "Ultron", "Dormammu"}, {"", "", ""}, {"", "", ""}
}; 

struct Question {
  String qText;
  String options[4];
  int correctIdx;
};

Question quizBank[4] = {
  {"Which is a microprocesser?", {"555 Timer", "STM32", "Raspberry Pi", "PIC"}, 1},
  {"What is the unit of current?", {"Volt", "Watt", "Ampere", "Ohm"}, 2},
  {"V=I x ?", {"R", "P", "C", "L"}, 0},
  {"Which Components stores charge?", {"Transistor", "Diode", "Capacitor", "Switch"}, 2}
};

int activeQIdx = -1;
String tempEventName = "";
String enteredUser = "";
String enteredPass = "";
String loggedName  = "";
int loggedIdx = -1; 
int winnerIndex = 0;
String enteredWinner = "";

const char* kbKeys = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
enum Screen { HOME, LOGIN_ID, LOGIN_PW, ADMIN_LOGIN, USER_DASHBOARD, ADMIN_DASHBOARD, EVENT_REG, WINNER_EVENT_PICK, WINNER_INPUT, SHOW_WINNERS, QUIZ_PLAY, USER_REG_PAGE, ADMIN_VIEW_REG };
Screen currentScreen = HOME;

int keyStartX = 65, keyStartY = 75, keyW = 62, keyH = 38;

void setup() {
  uint16_t id = tft.readID();
  tft.begin(id);
  tft.setRotation(1);
  randomSeed(analogRead(A5));
  drawHome();
}

void loop() {
  TSPoint p = ts.getPoint();
  pinMode(XM, OUTPUT); pinMode(YP, OUTPUT);
  if (p.z < MINPRESSURE || p.z > MAXPRESSURE) return;
  int x = map(p.y, TS_MINY, TS_MAXY, 0, tft.width());
  int y = map(p.x, TS_MINX, TS_MAXX, tft.height(), 0);
  
  if(hit(x,y,270,0,50,50)){ goBack(); delay(250); return; }

  switch(currentScreen){
    case HOME: handleHome(x,y); break;
    case LOGIN_ID: handleLoginID(x,y); break;
    case LOGIN_PW: handleLoginPW(x,y); break;
    case ADMIN_LOGIN: handleAdminLogin(x,y); break;
    case USER_DASHBOARD: handleUserDashboard(x,y); break;
    case ADMIN_DASHBOARD: handleAdminDashboard(x,y); break;
    case EVENT_REG: handleEventReg(x,y); break;
    case WINNER_EVENT_PICK: handleWinnerEventPick(x,y); break;
    case WINNER_INPUT: handleWinnerKeypad(x,y); break;
    case QUIZ_PLAY: handleQuizChoice(x,y); break;
    case USER_REG_PAGE: handleUserReg(x,y); break;
    case ADMIN_VIEW_REG: break; 
  }
  delay(150);
}

// ================= FONT & BUTTON HELPERS =================

void drawBigButton(int x, int y, int w, int h, String t, uint16_t c) {
  tft.fillRoundRect(x, y, w, h, 8, c);
  tft.drawRoundRect(x, y, w, h, 8, WHITE);
  tft.setTextColor(WHITE);
  int s = 2;
  if (t.length() > 18) s = 1; 
  tft.setTextSize(s);
  int textW = t.length() * (s * 6);
  int textH = (s * 8);
  tft.setCursor(x + (w - textW) / 2, y + (h - textH) / 2);
  tft.print(t);
}

void drawBack(){ tft.fillTriangle(305,10,305,30,285,20,WHITE); }
bool hit(int x,int y,int bx,int by,int bw,int bh){ return (x >= bx && x <= bx+bw && y >= by && y <= by+bh); }

// ================= HOME & LOGIN =================

void drawHome() {
  currentScreen = HOME; loggedName = ""; enteredUser = ""; enteredPass = ""; 
  tft.fillScreen(BLACK); tft.setTextColor(WHITE); tft.setTextSize(3);
  tft.setCursor(60,25); tft.println("QUIZ BOOTH &");
  tft.setCursor(40,65); tft.println("REGISTRATION");
  drawBigButton(40,165,110,55, "USER", GREEN);
  drawBigButton(170,165,110,55, "ADMIN", RED);
}

void handleHome(int x,int y){
  if(hit(x,y,40,165,110,55)) drawLoginID();
  if(hit(x,y,170,165,110,55)) drawAdminLogin();
}

void drawLoginID() {
  currentScreen = LOGIN_ID; tft.fillScreen(BLACK); drawBack();
  tft.drawRect(10, 10, 250, 55, WHITE);
  tft.setTextColor(WHITE); tft.setTextSize(2); tft.setCursor(20, 30); tft.print("ID: "); tft.print(enteredUser);
  drawKeypad();
}

void handleLoginID(int x,int y){
  if(handleKeyInput(x, y, enteredUser)) drawLoginID();
  if(hit(x,y,210,195,100,40)){ 
    for(int i=0; i<5; i++) if(enteredUser == usernames[i]) { loggedIdx = i; drawLoginPW(); return; } 
    enteredUser = ""; drawLoginID(); 
  }
}

void drawLoginPW() {
  currentScreen = LOGIN_PW; tft.fillScreen(BLACK); drawBack();
  tft.drawRect(10, 10, 250, 55, WHITE);
  tft.setTextColor(WHITE); tft.setTextSize(2); tft.setCursor(20, 30); tft.print("PW: "); for(int i=0; i<enteredPass.length(); i++) tft.print("*"); 
  drawKeypad();
}

void handleLoginPW(int x,int y){
  if(handleKeyInput(x, y, enteredPass)) drawLoginPW();
  if(hit(x,y,210,195,100,40)){ 
    if(enteredPass == passwords[loggedIdx]) { loggedName = names[loggedIdx]; drawUserDashboard(); } 
    else { enteredPass = ""; drawLoginPW(); } 
  }
}

void drawAdminLogin(){
  currentScreen = ADMIN_LOGIN; tft.fillScreen(BLACK); drawBack();
  tft.setTextColor(WHITE); tft.setTextSize(2); tft.setCursor(20,35); 
  tft.print("Admin Pin: "); tft.setTextColor(YELLOW); 
  for(int i=0; i < enteredPass.length(); i++) tft.print("*");
  drawKeypad();
}

void handleAdminLogin(int x,int y){
  if(handleKeyInput(x, y, enteredPass)) drawAdminLogin();
  if(hit(x,y,210,195,100,40)) {
    if(enteredPass == "9999") { enteredPass = ""; drawAdminDashboard(); }
    else { enteredPass = ""; drawAdminLogin(); }
  }
}

// ================= DASHBOARDS =================

void drawUserDashboard(){
  currentScreen = USER_DASHBOARD; tft.fillScreen(BLACK); drawBack();
  tft.setCursor(15, 10); tft.setTextColor(CYAN); tft.setTextSize(2); tft.print("Hi, "); tft.print(loggedName);
  tft.setCursor(15, 40); tft.setTextColor(YELLOW); tft.print("Score: "); tft.print(userScores[loggedIdx]);
  tft.setCursor(15, 65); tft.setTextSize(1);
  if(hasAttendedQuiz[loggedIdx]) { tft.setTextColor(RED); tft.print("QUIZ: COMPLETED"); }
  else { tft.setTextColor(GREEN); tft.print("QUIZ: AVAILABLE"); }
  drawBigButton(10, 85, 145, 65, "VIEW WINS", BLUE);
  uint16_t quizCol = hasAttendedQuiz[loggedIdx] ? 0x7BEF : BLUE; 
  drawBigButton(165, 85, 145, 65, "START QUIZ", quizCol);
  drawBigButton(40, 160, 240, 65, "REGISTRATION", BLUE); 
}

void handleUserDashboard(int x,int y){
  if(hit(x,y,10,85,145,65)) { eventPage = 0; drawWinnerEventPick(); }
  if(hit(x,y,165,85,145,65)) { if(!hasAttendedQuiz[loggedIdx]) { activeQIdx = random(0, 4); currentScreen = QUIZ_PLAY; drawQuizScreen(); } }
  if(hit(x,y,40,160,240,65)) { drawUserRegPage(); }
}

void drawAdminDashboard(){
  currentScreen = ADMIN_DASHBOARD; tft.fillScreen(BLACK); drawBack();
  tft.setTextColor(CYAN); tft.setTextSize(2); tft.setCursor(15,10); tft.print("Admin Panel");
  drawBigButton(10, 50, 145, 65, "REG EVENT", BLUE);
  drawBigButton(165, 50, 145, 65, "SET WINNER", BLUE);
  drawBigButton(40, 135, 240, 65, "REGISTRATIONS", BLUE); 
}

void handleAdminDashboard(int x,int y){
  if(hit(x,y,10,50,145,65)){ tempEventName=""; drawEventReg(); }
  if(hit(x,y,165,50,145,65)) { eventPage = 0; drawWinnerEventPick(); }
  if(hit(x,y,40,135,240,65)) { drawAdminViewReg(); }
}

// ================= REGISTRATION LOGIC =================

void drawUserRegPage() {
  currentScreen = USER_REG_PAGE; tft.fillScreen(BLACK); drawBack();
  tft.setTextColor(CYAN); tft.setTextSize(2); tft.setCursor(10, 20); tft.print("UPCOMING EVENT:");
  tft.setTextColor(YELLOW); tft.setTextSize(2); tft.setCursor(10, 55); tft.print("Transistor Battle");
  drawBigButton(40, 120, 240, 75, "REGISTER NOW", BLUE);
}

void handleUserReg(int x, int y) {
  if(hit(x,y,40,120,240,75)) {
    bool alreadyReg = false;
    for(int i=0; i<regCount; i++) if(registeredUsers[i].id == usernames[loggedIdx]) alreadyReg = true;
    
    tft.fillScreen(BLACK); tft.setCursor(40, 100); tft.setTextSize(2);
    if(!alreadyReg && regCount < 20) {
      registeredUsers[regCount].name = loggedName;
      registeredUsers[regCount].id = usernames[loggedIdx];
      regCount++;
      tft.setTextColor(GREEN); tft.print("SUCCESSFULLY REG!");
    } else {
      tft.setTextColor(RED); tft.print("ALREADY REGISTERED");
    }
    delay(2000); drawUserDashboard();
  }
}

void drawAdminViewReg() {
  currentScreen = ADMIN_VIEW_REG; tft.fillScreen(BLACK); drawBack();
  tft.setTextColor(CYAN); tft.setTextSize(1); tft.setCursor(10, 5); tft.print("EVENT: TRANSISTOR BATTLE");
  tft.drawLine(0, 20, 320, 20, WHITE);
  
  tft.setTextColor(YELLOW); tft.setTextSize(2);
  tft.setCursor(10, 25); tft.print("REGISTRATIONS:");
  
  tft.setTextSize(1); tft.setTextColor(WHITE);
  for(int i=0; i<regCount; i++) {
    tft.setCursor(15, 55 + (i * 18));
    // Shows "Name [Username]"
    tft.print(String(i+1) + ". " + registeredUsers[i].name + " [" + registeredUsers[i].id + "]");
  }
  if(regCount == 0) { tft.setCursor(15, 60); tft.print("No entries yet."); }
}

// ================= EVENT & WINNER LOGIC =================

void drawEventReg(){
  currentScreen = EVENT_REG; tft.fillScreen(BLACK); drawBack();
  tft.setTextColor(CYAN); tft.setTextSize(1); tft.setCursor(10,5); tft.print("ADMIN: EVENT REGISTRATION");
  tft.drawRect(5, 20, 260, 35, WHITE);
  tft.setTextColor(YELLOW); tft.setTextSize(2); tft.setCursor(12, 30); tft.print(tempEventName + "_");
  int kw = 34, kh = 35;
  for(int i=0; i<26; i++){
    int col = i % 9; int row = i / 9;
    drawBigButton(5 + col*kw, 60 + row*kh, kw-3, kh-3, String(kbKeys[i]), 0x2104); 
  }
  drawBigButton(5, 175, 70, 40, "DEL", RED);
  drawBigButton(80, 175, 115, 40, "SPACE", BLUE);
  drawBigButton(200, 175, 75, 40, "SAVE", GREEN);
}

void handleEventReg(int x,int y){
  int kw = 34, kh = 35;
  for(int i=0; i<26; i++){
    int col = i % 9; int row = i / 9;
    if(hit(x, y, 5 + col*kw, 60 + row*kh, kw-3, kh-3)){ if(tempEventName.length() < 18) tempEventName += kbKeys[i]; drawEventReg(); return; }
  }
  if(hit(x,y,80, 175, 115, 40)){ tempEventName += " "; drawEventReg(); }
  if(hit(x,y,5, 175, 70, 40)){ if(tempEventName.length()>0) tempEventName.remove(tempEventName.length()-1); drawEventReg(); }
  if(hit(x,y,200, 175, 75, 40)) if(eventCount < 15 && tempEventName.length() > 1){ events[eventCount++] = tempEventName; drawAdminDashboard(); }
}

void drawWinnerEventPick(){
  currentScreen = WINNER_EVENT_PICK; tft.fillScreen(BLACK); drawBack();
  tft.setCursor(15,10); tft.setTextColor(WHITE); tft.setTextSize(2); tft.print("Select Event:");
  int startIdx = eventPage * 3; 
  for(int i=0; i<3; i++){
    int cur = startIdx + i;
    if(cur < eventCount) drawBigButton(10, 45 + (i * 60), 300, 55, events[cur], BLUE); 
  }
  if(eventPage > 0) drawBigButton(10, 220, 90, 30, "BACK", RED);
  if((eventPage + 1) * 3 < eventCount) drawBigButton(220, 220, 90, 30, "NEXT", RED);
}

void handleWinnerEventPick(int x,int y){
  int startIdx = eventPage * 3;
  for(int i=0; i<3; i++){
    int cur = startIdx + i;
    if(cur < eventCount && hit(x,y,10, 45 + (i * 60), 300, 55)){
      selectedEventIdx = cur;
      if(loggedName == ""){ winnerIndex=0; enteredWinner=""; drawWinnerInput(); }
      else showAllWinners(selectedEventIdx);
      return;
    }
  }
  if(eventPage > 0 && hit(x,y,10, 220, 90, 30)) { eventPage--; drawWinnerEventPick(); }
  if(((eventPage + 1) * 3 < eventCount) && hit(x,y,220, 220, 90, 30)) { eventPage++; drawWinnerEventPick(); }
}

void drawWinnerInput(){
  currentScreen = WINNER_INPUT; tft.fillScreen(BLACK); drawBack();
  tft.setTextColor(WHITE); tft.setTextSize(2); tft.setCursor(10,10); tft.print(events[selectedEventIdx]);
  tft.setCursor(10,35); tft.print("Rank " + String(winnerIndex+1) + " ID: " + enteredWinner);
  drawKeypad();
}

void handleWinnerKeypad(int x,int y){
  if(handleKeyInput(x, y, enteredWinner)) drawWinnerInput();
  if(hit(x,y,210,195,100,40)) for(int i=0; i<5; i++) if(enteredWinner == usernames[i]){ 
    USERS_WINNERS[selectedEventIdx][winnerIndex++] = names[i]; enteredWinner = ""; 
    if(winnerIndex >= 3) { showAllWinners(selectedEventIdx); return; } 
    drawWinnerInput(); return; 
  }
}

void showAllWinners(int idx){
  currentScreen = SHOW_WINNERS; tft.fillScreen(BLACK); drawBack();
  tft.setTextColor(YELLOW); tft.setTextSize(2); tft.setCursor(15,15); tft.print("Winners: " + events[idx]);
  for(int i=0; i<3; i++){ 
    tft.setCursor(30, 65 + (i*40)); tft.setTextColor(WHITE); tft.print(String(i+1) + "st: "); 
    tft.setTextColor(CYAN); tft.print(USERS_WINNERS[idx][i] == "" ? "---" : USERS_WINNERS[idx][i]); 
  }
}

// ================= QUIZ LOGIC =================

void drawQuizScreen() {
  tft.fillScreen(BLACK); drawBack();
  tft.setTextColor(WHITE); tft.setTextSize(2);
  tft.setCursor(10, 15); tft.println(quizBank[activeQIdx].qText);
  for(int i=0; i<4; i++) {
    int bx = (i < 2) ? 10 : 165; int by = (i % 2 == 0) ? 80 : 150;
    drawBigButton(bx, by, 145, 55, quizBank[activeQIdx].options[i], BLUE);
  }
}

void handleQuizChoice(int x, int y) {
  int choice = -1;
  if(hit(x,y,10,80,145,55)) choice = 0; else if(hit(x,y,10,150,145,55)) choice = 1;
  else if(hit(x,y,165,80,145,55)) choice = 2; else if(hit(x,y,165,150,145,55)) choice = 3;
  if(choice != -1) {
    hasAttendedQuiz[loggedIdx] = true;
    if(choice == quizBank[activeQIdx].correctIdx) {
      userScores[loggedIdx] += 10;
      tft.fillScreen(GREEN); tft.setCursor(60, 100); tft.setTextColor(WHITE); tft.setTextSize(3); tft.print("CORRECT!");
    } else {
      tft.fillScreen(RED); tft.setCursor(80, 100); tft.setTextColor(WHITE); tft.setTextSize(3); tft.print("WRONG!");
    }
    delay(1500); drawUserDashboard();
  }
}

// ================= KEYPAD & NAVIGATION =================

bool handleKeyInput(int x, int y, String &inputVar) {
  for(int i=1; i<=9; i++){
    int bx = keyStartX + ((i-1)%3)*keyW; int by = keyStartY + ((i-1)/3)*keyH;
    if(hit(x,y,bx,by,keyW-5,keyH-5)){ inputVar += String(i); return true; }
  }
  if(hit(x,y,keyStartX+keyW, keyStartY+3*keyH, keyW-5, keyH-5)){ inputVar += "0"; return true; }
  if(hit(x,y,10,195,90,40)){ if(inputVar.length()>0) inputVar.remove(inputVar.length()-1); return true; }
  return false;
}

void drawKeypad(){
  for(int r=0; r<3; r++) for(int c=0; c<3; c++) drawBigButton(keyStartX+c*keyW, keyStartY+r*keyH, keyW-8, keyH-8, String(r*3+c+1), BLACK);
  drawBigButton(keyStartX+keyW, keyStartY+3*keyH, keyW-8, keyH-8, "0", BLACK);
  drawBigButton(10, 195, 90, 40, "DEL", RED);
  drawBigButton(210, 195, 100, 40, "OK", GREEN);
}

void goBack(){
  switch(currentScreen){
    case LOGIN_ID: case ADMIN_LOGIN: case ADMIN_DASHBOARD: case USER_DASHBOARD: drawHome(); break;
    case WINNER_INPUT: case WINNER_EVENT_PICK: case ADMIN_VIEW_REG: drawAdminDashboard(); break;
    case EVENT_REG: drawAdminDashboard(); break;
    case SHOW_WINNERS: case USER_REG_PAGE: if(loggedName != "") drawUserDashboard(); else drawAdminDashboard(); break;
    case QUIZ_PLAY: drawUserDashboard(); break;
    default: drawHome(); break;
  }
}
