# MyHiveMind

## How to install
1. git clone https://github.com/Akara4ok/MyHiveMind.git
2. git clone https://gitlab.com/kzotkin/hiveemulator.git

============================

3. mv MyHiveMind/Deployment/docker-compose.yml ./
4. mv MyHiveMind/Deployment/CC_Dockerfile ./hiveemulator/src/CommunicationControl/DevOpsProject/Dockerfile
5. mv MyHiveMind/Deployment/CC_appsettings.json ./hiveemulator/src/CommunicationControl/DevOpsProject/appsettings.json
6. mv MyHiveMind/Deployment/Client_Dockerfile ./hiveemulator/src/MapClient/Dockerfile

=============================

7. cd hiveemulator/src/MapClient/
8. npm install
9. cd ../../../

=============================

10. docker compose build
    *. docker rm $(docker ps -aq) (optional)
11. docker compose up