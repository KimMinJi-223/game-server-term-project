myid = 99999
spawnPos = {x = 0, y = 0}
monsterType = -1
radarArea = 0; -- 현재 위치에서 어디까지 포착가능한가?
target = -1; -- 타겟이 없으면 -1

-- id와 type만 주면 알아서 다 맞추게 한다. 
function set_init(id, x, y, type)
   myid = id;
   monsterType = type;
   spawnPos.x = x;
   spawnPos.y = y;
   if type == 2 then
      radarArea = 0;
   elseif type == 3 then
      radarArea = 0;
   elseif type == 4 then
      radarArea = 5;
   elseif type == 5 then
      radarArea = 5;
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
        return
    end
    	

    --타겟이라면? 
    --로그인 상태도 확인해야함
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
        API_AddTimer(myid, playerId, 1, 1000);
        return
    end
end
