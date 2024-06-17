myid = 99999
spawnPos = {x = 0, y = 0}
monsterType = -1
radarArea = 0; -- ���� ��ġ���� ������ ���������Ѱ�?
target = -1; -- Ÿ���� ������ -1

-- id�� type�� �ָ� �˾Ƽ� �� ���߰� �Ѵ�. 
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
    -- Ÿ���� ���ٸ�
    if(target == -1) then 
        my_x = API_GetPosX(myid);
        my_y = API_GetPosY(myid);

        if ((my_x - x) * (my_x - x) > radarArea * radarArea) then
            return
	    end
	        if ((my_y - y) * (my_y - y) > radarArea * radarArea) then
            return
        end
        -- Ÿ�� Ȱ��ȭ
        API_AStarStart(myid, playerId);
        -- 1 : A��Ÿ �̵�
        API_AddTimer(myid, playerId, 1,  1000);
        target = playerId;
        return
    end
    	

    --Ÿ���̶��? 
    --�α��� ���µ� Ȯ���ؾ���
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
        -- 1 : A��Ÿ �̵�
        API_AddTimer(myid, playerId, 1, 1000);
        return
    end
end
