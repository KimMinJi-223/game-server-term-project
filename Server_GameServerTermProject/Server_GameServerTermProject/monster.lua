myid = 99999
spawnPos = {x = 0, y = 0}
monsterType = -1
moveState = 0; -- 0 : 정지, 1 : 영역 랜덤 무브, 2 : A*(공격당한 상태)
roamingArea = 0 -- 돌아다니는 영역
radarArea = 0; -- 현재 위치에서 어디까지 포착가능한가?
followArea = 0; -- 포작을 했으면 스폰지점부터 어디까지 따라갈거냐?
target = -1; -- 타겟이 없으면 -1

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

function IsAStar(playerId, x, y)
    -- 타겟이 없다면
    if(target == -1) then 
        my_x = API_GetPosX(myid);
        my_y = API_GetPosY(myid);

        if ((my_x - x) * (my_x - x) > radarArea * radarArea) then
            return
	    end
	        if ((my_y - y) * (my_y - y) > radarArea * radarArea) then
            return
        end
        -- 타겟 활성화
        API_AStarStart(myid, playerId);
        -- 1 : A스타 이동
        API_AddTimer(myid, playerId, 1,  1000);
        target = playerId;
    end

    --타겟이라면?
    if(playerId == target) then 
        my_x = API_GetPosX(myid);
        my_y = API_GetPosY(myid);

        if ((my_x - x) * (my_x - x) > radarArea * radarArea) then
            API_AStarEnd(myid);
            target = -1;
            return
	    end
	    if ((my_y - y) * (my_y - y) > radarArea * radarArea) then
            API_AStarEnd(myid);
            target = -1
            return
        end
        -- 1 : A스타 이동
        --API_AddTimer(myid, playerId, 1, 1000);
    end
end
