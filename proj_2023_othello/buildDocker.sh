GREEN='\033[1;32m'
NC='\033[0m'

# Stop the current ingenious container if it is running
printf "\n${GREEN}Stopping current container...${NC}\n"
docker stop ingeniousframe

# Export current containers files to prevent accidental loss of data
# echo "Saving current container to exports/prevContainer.tar..."
# docker export -o exports/prevContainer.tar ingeniousframe

# Remove the current container
printf "\n${GREEN}Removing current container...${NC}\n"
docker container rm ingeniousframe

# Build new docker image
printf "\n${GREEN}Building new docker image...${NC}\n"
docker build --no-cache -t ingenious-frame IngeniousFrame/

# Remove old docker image
printf "\n${GREEN}Removing old docker image...${NC}\n"
echo "y" | docker image prune
docker rmi $(docker images -f "dangling=true" -q)

# Create and run a new ingenious-frame container
printf "\n${GREEN}Creating new container...${NC}\n"

# The `--network host` flag might not be needed
# The `--memory=300g` flag might not be needed
docker run -dit \
    --network host \
    --memory=300g \
    --name ingeniousframe \
    ingenious-frame
echo "Done."
