ifeq ($(origin .RECIPEPREFIX), undefined)
  $(error This Make does not support .RECIPEPREFIX. Please use GNU Make 4.0 or later)
endif
.RECIPEPREFIX = >

DOCKER_IMAGE = ssd1306ascii-builder
DOCKER_USER = -u $(shell id -u):$(shell id -g)
DOCKER_BUILD = docker build --rm -f docker/Dockerfile --target builder -t $(DOCKER_IMAGE) .
DOCKER_RUN = docker run --rm -t -v ${PWD}:/build -w /build $(DOCKER_USER) $(DOCKER_IMAGE):latest

default: build

docker:
> $(DOCKER_BUILD)

build:
> $(DOCKER_RUN) scripts/build.sh

ci: docker build example

clean:
> rm -rf build
> rm -rf example/build

install: clean
> scripts/install.sh

example:
> docker run --rm -t -v ${PWD}:/build -w /build $(DOCKER_IMAGE):latest scripts/build-example.sh

.PHONY: default ci build docker clean install example
