MIN=yui-compressor
TIME=/usr/bin/time --format="Build Duration: %E" 

#directories
JS_DIR=html/js

CSS_DIR=html/css

all:
	$(TIME) make all-act


#wildcards

#x_NO_WANT: already existing minified files we want to exclude from sources
#x_SRCS: sources
#x_MIN: paths of the minified sources

JS_NO_WANT := $(wildcard $(JS_DIR)/*-min.js)
JS_SRCS := $(filter-out $(JS_NO_WANT),$(wildcard $(JS_DIR)/*.js))
JS_MIN := $(patsubst %.js,%-min.js,$(JS_SRCS))

CSS_NO_WANT := $(wildcard $(CSS_DIR)/*-min.css)
CSS_SRCS := $(filter-out $(CSS_NO_WANT),$(wildcard $(CSS_DIR)/*.css))
CSS_MIN := $(patsubst %.css,%-min.css,$(CSS_SRCS))


all-act: $(JS_MIN) $(CSS_MIN)


#minify the js files
$(JS_DIR)/%-min.js: $(JS_DIR)/%.js
	$(MIN) -o $@ $<

#minify the css files
$(CSS_DIR)/%-min.css: $(CSS_DIR)/%.css
	$(MIN) -o $@ $<


clean:
	rm $(JS_NO_WANT) $(CSS_NO_WANT)

.PHONY: all clean