myid = 99999
spawnPos = {x = 0, y = 0}
monsterType = -1
moveState = 0; -- 0 : ����, 1 : ���� ���� ����, 2 : A*(���ݴ��� ����)
roamingArea = 0 -- ���ƴٴϴ� ����
radarArea = 0; -- ���� ��ġ���� ������ ���������Ѱ�?
followArea = 0; -- ������ ������ ������������ ������ ���󰥰ų�?
target = -1; -- Ÿ���� ������ -1

-- id�� type�� �ָ� �˾Ƽ� �� ���߰� �Ѵ�. 
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
    end

    --Ÿ���̶��?
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
        --API_AddTimer(myid, playerId, 1, 1000);
    end
end
