void setup() {
  // put your setup code here, to run once:

}
bool isRome, imRome, turning, st, tover, timing, ending;
byte turn, players, val, count = 5;
byte vals[6];
Color cols[5] = {OFF, RED, BLUE, GREEN, MAGENTA};
Timer time;
void loop(){
  if(!isRome){ //build Rome
    if(buttonDoubleClicked()){
      init(2);
    }else if(buttonMultiClicked()){
      init(min(buttonClickCount(), 4));
    }
  }else{
    if(!ending && !turning && !st && buttonSingleClicked()){
      turning = st = true;
      setValueSentOnAllFaces(turn);
    }else if(imRome && time.isExpired() && buttonLongPressed()){ //reset the entire board
      reset();
      setValueSentOnAllFaces(18);
    }
  }
  if(timing && time.isExpired()){
    turning = timing = false;
    turn = turn % players + 1;
    if(turn == 1 && --count == 0){ //end the game
      setValueSentOnAllFaces(17);
      ending = true;
      time.set(1000);
    }else setValueSentOnAllFaces(turn + 11);
  }
  FOREACH_FACE(f){
    if(didValueOnFaceChange(f)) switch(val = getLastValueReceivedOnFace(f)){ //mesage reciever
      case 0: break;
      case 1: case 2: case 3: case 4: //relay's signal out
        if(block() && vals[0] != val) setValueSentOnFace(16, f); //bounceback clear
        else if(has(val) > 0 || imRome){
          if(!imRome){
            vals[f] = val;
            vals[opp(f)] = val;
          }
          setValueSentOnFace(val+7, f); //bounceback
        }else if(isValueReceivedOnFaceExpired(opp(f))){ //bounceback clear
          setValueSentOnFace(16, f);
        }else{
          setValueSentOnFace(val, opp(f)); //continue through
          st = true;
        }
      break; case 5: case 6: case 7: //spread word of Rome's establishment (and how many players there are)
        if(!isRome){
          players = val-3;
          setValueSentOnAllFaces(val);
          turn = 1;
          isRome = true;
        }
      break; case 8: case 9: case 10: case 11: //return to relay
        if(st || turning){
          vals[f] = val-7;
          vals[opp(f)] = val-7;
          if(!turning) setValueSentOnFace(val, opp(f));
          else{
            time.set(500);
            timing = true;
          }
        }
        tover = true;
      break; case 12: case 13: case 14: case 15:
        if(val - 11 > turn || (turn == players && val == 12)){
          turn = val - 11;
          if(turn == 1 && --count == 0){
            setValueSentOnAllFaces(17);
            ending = true;
            time.set(1000);
          }else{
            turning = st = false;
            setValueSentOnAllFaces(val);
          }
        }
      break; case 16: //clear this tile from step consideration without laying road
        if(!turning) setValueSentOnFace(16, opp(f));
        tover = true;
      break; case 17: //game end
        if(!ending){
          setValueSentOnAllFaces(val);
          ending = true;
          time.set(1000);
        }
      break; case 18: //game reset
        if(isRome){
          setValueSentOnAllFaces(val);
          reset();
        }
      break;
    }
    if(ending && !time.isExpired()) setColor(WHITE);
    else if(st) setColor(ORANGE);
    else if(!imRome) setColorOnFace(cols[vals[f]], f);
    else setColor(YELLOW);
  }
  if(tover) st = tover = false;
}
byte has(byte c){
  byte r = 0;
  FOREACH_FACE(f) if(vals[f] == c) r++;
  return r;
}
byte opp(byte f){
  return (f+3)%6;
}
bool block(){
  bool r = vals[0] != 0;
  if(r) FOREACH_FACE(f) r = r && vals[f] == vals[0];
  return r;
}
void reset(){
  isRome = imRome = turning = st = tover = timing = ending = false;
  turn = players = val = 0;
  count = 5;
  FOREACH_FACE(f) vals[f] = 0;
}
void init(byte b){
  isRome = imRome = true;
  turn = 1; players = b;
  setValueSentOnAllFaces(players+3);
}
