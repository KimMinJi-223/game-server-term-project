myid = 99999
spawnPos = {x = 0, y = 0}
monsterType = -1
moveState = 0; -- 0 : 정지, 1 : 영역 랜덤 무브, 2 : A*(공격당한 상태)
roamingArea = 0 -- 돌아다니는 영역
radarArea = 0; -- 현재 위치에서 어디까지 포착가능한가?
followArea = 0; -- 포작을 했으면 스폰지점부터 어디까지 따라갈거냐?

-- id와 type만 주면 알아서 다 맞추게 한다. 
function set_init(id, x, y, type)
   myid = id;
   monsterType = type;
   spawnPos.x = x;
   spawnPos.y = y;
   if type == 2 then
      moveState = 0;
      roamingArea = 0;
      radarArea = 0;
      followArea = 30;
   elseif type == 3 then
      moveState = 1;
      roamingArea = 20;
      radarArea = 0;
      followArea = 30;
   elseif type == 4 then
      moveState = 0;
      roamingArea = 0;
      radarArea = 5;
      followArea = 30;
   elseif type == 5 then
      moveState = 1;
      roamingArea = 20;
      radarArea = 5;
      followArea = 30;
   end
   --print(moveState, roamingArea, radarArea, followArea);
end

-- 타입만주고 무브 함수에서 타입에 따라 함수를 호출하게 하자
-- 20 20 영역에서 움직이고 11영역안에 들어오면 스폰 위치에서 30영역까지 따라가기 
-- 가만히 있는 몬스터도 30영역까지 따라가기
function IsAStar(playerId, x, y)
    my_x = API_GetPosX(myid);
    my_y = API_GetPosY(myid);
    if ((my_x - x) * (my_x - x) > radarArea * radarArea) then
    API_OkAStar(myid, -1);
    return
	end
	if ((my_y - y) * (my_y - y) > radarArea * radarArea) then
    API_OkAStar(myid, -1);
    return
    end
    API_OkAStar(myid, playerId);
end
