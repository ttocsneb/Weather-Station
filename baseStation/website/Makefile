MIN=uglifyjs --compress --mangle
MINCSS=yui-compressor
TIME=/usr/bin/time --format="Build Duration: %E" 

#directories
DIR=html

EXCLUDE_DIR=html/js/includes html/startbootstrap-sb-admin

all:
	$(TIME) make all-act


#wildcards

# x_NO_WANT: already existing minified files we want to exclude from sources
# x_CLEAN: Files that we would remove if we wanted to clean
# x_SRCS: sources
# x_MIN: paths of the minified sources

JS_NO_WANT := $(shell find $(DIR) -name '*.min.js') $(shell find $(EXCLUDE_DIR) -name '*.js' -prune -type f)
JS_CLEAN := $(filter-out $(shell find $(EXCLUDE_DIR)),$(JS_NO_WANT))
JS_SRCS := $(filter-out $(JS_NO_WANT),$(shell find $(DIR) -name '*.js' -prune -type f))
JS_MIN := $(patsubst %.js,%.min.js,$(JS_SRCS))

CSS_NO_WANT := $(shell find $(DIR) -name '*.min.css') $(shell find $(EXCLUDE_DIR) -name '*.css' -prune -type f)
CSS_CLEAN := $(filter-out $(shell find $(EXCLUDE_DIR)),$(CSS_NO_WANT))
CSS_SRCS := $(filter-out $(CSS_NO_WANT),$(shell find $(DIR) -name '*.css' -prune -type f))
CSS_MIN := $(patsubst %.css,%.min.css,$(CSS_SRCS))

all-act: $(JS_MIN) $(CSS_MIN)


#minify the js files
%.min.js: %.js
	$(MIN) -o $@ -- $<

# minify the css files
%.min.css: %.css
	$(MINCSS) -o $@ $<


clean:
	rm $(JS_CLEAN) $(CSS_CLEAN)

.PHONY: all clean