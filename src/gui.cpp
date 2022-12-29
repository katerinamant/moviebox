#include "app.h"
#include "defines.h"
#include "moviedb.h"

/* Home Screen */

void HomeScreen::draw() {
    if (APP()->getScreen() != "Home") return;

    graphics::Brush br;
    br.outline_opacity = 0.0f;
    graphics::drawText(CANVAS_WIDTH/6.0f + 5.0/9.0f, (CANVAS_HEIGHT/4.0f + 3.25f) + 1.2f, 0.5f, "Popular", br);
    graphics::drawText(CANVAS_WIDTH/6.0f + 5.0/9.0f, (CANVAS_HEIGHT/4.0f + 3.25f) + 5.45f, 0.5f, "New Releases", br);

    for (auto w : m_widgets) {
        w->draw();
    }

    if (m_active_button) {
        APP()->setScreen("Movie");
        APP()->setMovie(m_active_button->getMovie());
    }
    m_active_button = nullptr;
}

void HomeScreen::update() {
    if (APP()->getScreen() != "Home") return;

    // Get mouse position
    graphics::MouseState ms;
    graphics::getMouseState(ms);

    // Convert mouse cords to canvas cords
    float mx = graphics::windowToCanvasX(ms.cur_pos_x);
    float my = graphics::windowToCanvasY(ms.cur_pos_y);

    // Highlight button
    MovieButton* cur_button = nullptr;
    for (auto b : m_buttons) {
        if (b->contains(mx, my)) {
            b->setHighlight(true);
            cur_button = b;
        } else {
            b->setHighlight(false);
        }
    }

    // Activate movie
    if (ms.button_left_pressed && cur_button) {
        m_active_button = cur_button;
        m_active_button->setActive(true);
        for (auto b : m_buttons) {
            if (b != m_active_button) {
                b->setActive(false);
            }
        }
    }

    for (auto w : m_widgets) {
        w->update();
    }
}

void HomeScreen::init() {
    Slideshow* s = new Slideshow();
    m_widgets.push_back((Widget*) s);
    s->setPosX(m_pos[0]);
    s->setPosY(CANVAS_HEIGHT/4.0f);
    s->init();

    for (int i=0; i<2; i++) {
        std::vector<Movie*> home_movies = (i==0) ? DB()->getPopularMovies() : DB()->getMoviesFromRange(2021,2022);
        for (int j=0; j<5; j++) {
            MovieButton* m = new MovieButton();
            m_widgets.push_back((Widget*) m);
            m_buttons.push_back(m);
            m->setMovie(home_movies[j]);
            m->setText("Horizontal");
            m->setPosX(CANVAS_WIDTH/6.0f + 5.0/9.0f + 2.0f + (4.0f + 5.0/9.0f)*j);
            m->setPosY((s->getPosY()+3.25f) + 2.75f + 4.25*i);
        }
    }
}

HomeScreen::~HomeScreen() {
    for (auto w : m_widgets) {
        delete w;
    }
    m_widgets.clear();
}

/* Slide Show */

void Slideshow::draw() {
    // Draw slideshow banner pic
    std::vector<Movie*> movies = DB()->getSlideshowMovies();
    graphics::Brush br;
    br.fill_opacity = 1.0f;
    br.outline_opacity = 0.0f;

    switch (m_slide) {
        case 1:
            br.texture = ASSET_PATH + std::string(movies[0]->getShortcut() + ".png");
            break;
        case 2:
            br.texture = ASSET_PATH + std::string(movies[1]->getShortcut() + ".png");
            break;
        case 3:
            br.texture = ASSET_PATH + std::string(movies[2]->getShortcut() + ".png");
            break;
        case 4:
            br.texture = ASSET_PATH + std::string(movies[3]->getShortcut() + ".png");
            break;
        default:
            SETCOLOR(br.fill_color, 1.0f, 1.0f, 1.0f);
            break;
    }

    // Add gradient
    SETCOLOR(br.fill_secondary_color, 0.03f, 0.04f, 0.12f);
    br.fill_secondary_opacity = 1.0f;
    br.gradient = true;
    br.gradient_dir_u = 0.0f;
    br.gradient_dir_v = 1.0f;
    graphics::drawRect(m_pos[0], m_pos[1], CANVAS_WIDTH * 5.0f/8.0f, 6.5f, br);

    // Draw slideshow buttons
    for (auto b : m_buttons) {
        b->draw();
    }
}

void Slideshow::update() {
    // Get mouse position
    graphics::MouseState ms;
    graphics::getMouseState(ms);

    // Convert mouse cords to canvas cords
    float mx = graphics::windowToCanvasX(ms.cur_pos_x);
    float my = graphics::windowToCanvasY(ms.cur_pos_y);

    // Highlight button
    SlideshowButton* cur_button = nullptr;
    for (auto b : m_buttons) {
        if (b->contains(mx, my)) {
            b->setHighlight(true);
            cur_button = b;
        } else {
            b->setHighlight(false);
        }
    }

    // Change to next or prev slide
    if (ms.button_left_pressed && cur_button) {
        if (cur_button->getText() == "Left") {
            m_slide = m_slide<=1 ? 4 : m_slide-1;
        } else {
            m_slide = m_slide>=4 ? 1 : m_slide+1;
        }
    }

    // Call update on dependent members: buttons
    for (auto b : m_buttons) {
        b->update();
    }
}

void Slideshow::init() {
    SlideshowButton* l = new SlideshowButton();
    m_buttons.push_front(l);
    l->setText("Left");
    l->setIcon(ASSET_PATH + std::string("left.png"));
    l->setPosX(m_pos[0] - 1.5f - CANVAS_WIDTH * 5.0f/16.0f);
    l->setPosY(m_pos[1]);

    SlideshowButton* r = new SlideshowButton();
    m_buttons.push_front(r);
    r->setText("Right");
    r->setIcon(ASSET_PATH + std::string("right.png"));
    r->setPosX(m_pos[0] + 1.5f + CANVAS_WIDTH * 5.0f/16.0f);
    r->setPosY(m_pos[1]);
}

Slideshow::~Slideshow() {
    for (auto b : m_buttons) {
        delete b;
    }
    m_buttons.clear();
}


/* Slideshow Buttons */

void SlideshowButton::draw() {
    graphics::Brush br;
    br.fill_opacity = 1.0f;
    br.outline_opacity = 0.0f;
    br.texture = m_icon;

    if (m_highlighted) {
        SETCOLOR(br.fill_color, 1.0f, 0.54f, .0f);
    } else {
        SETCOLOR(br.fill_color, 0.80f, 0.80f, 0.85f);
    }
    graphics::drawRect(m_pos[0], m_pos[1], 1.0f, 1.0f, br);
}

void SlideshowButton::update() {

}

bool SlideshowButton::contains(float x, float y) {
    return x < m_pos[0] + 0.5f && x > m_pos[0] - 0.5f && y > m_pos[1] - 0.5f && y < m_pos[1] + 0.5f;
}


/* Movie Widget */

void MovieButton::draw() {
    graphics::Brush br;
    br.outline_opacity = 0.0f;
    if (m_text == "Horizontal") {
        br.texture = m_movie->getImg();
        graphics::drawRect(m_pos[0], m_pos[1], 4.0f, 2.5f, br);
    } else if (m_text == "Vertical") {
        br.texture = m_movie->getPoster();
        graphics::drawRect(m_pos[0], m_pos[1], 4.375f, 6.25f, br);
    }

    SETCOLOR(br.fill_color, 0.0f, 0.0f, 0.0f);
    br.fill_opacity = 0.4f * m_highlighted;
    if (m_text == "Horizontal") {
        graphics::drawRect(m_pos[0], m_pos[1], 4.0f, 2.5f, br);
    } else if (m_text == "Vertical") {
        graphics::drawRect(m_pos[0], m_pos[1], 4.375f, 6.25f, br);
    }
}

void MovieButton::update() {

}

bool MovieButton::contains(float x, float y) {
    return (m_text == "Horizontal") ? m_pos[0] - 2.0f <= x && x <= m_pos[0] + 2.0f && m_pos[1] - 1.25f <= y && y <= m_pos[1] + 1.25f :
                                    m_pos[0] - 2.1875f <= x && x <= m_pos[0] + 2.1875f && m_pos[1] - 3.125f <= y && y <= m_pos[1] + 3.125f;
}

/* Browse Screen */

void BrowseScreen::draw() {
    if (APP()->getScreen() != "Browse") return;

    graphics::Brush br;
    br.outline_opacity = 0.0f;

    // Draw slider info
    SETCOLOR(br.fill_color, 0.80f, 0.80f, 0.85f);
    graphics::drawText(CANVAS_WIDTH/6.0f + 5.0/9.0f, 1.0f, 0.4f, "From: ", br);
    graphics::drawText(CANVAS_WIDTH/6.0f + 5.0/9.0f + 5.5f, 1.0f, 0.4f, "1980", br);
    graphics::drawText(CANVAS_WIDTH/6.0f + 5.0/9.0f, 2.0f, 0.4f, "To: ", br);
    graphics::drawText(CANVAS_WIDTH/6.0f + 5.0/9.0f + 5.5f, 2.0f, 0.4f, "2022", br);

    std::vector<Movie*> results = DB()->getMoviesFromRange(1980, 2022);
    int counter = 0;
    for (int i=0; i<2; i++) {
        for (int j=0; j<5; j++) {
            if(counter >= results.size()) break;
            MovieButton* m = new MovieButton();
            //m_widgets.push_back((Widget*) m); //error in destructor (deleted again later)
            m_buttons.push_back((Button*) m);
            m->setMovie(results[counter]);
            m->setText("Vertical");
            m->setPosX(CANVAS_WIDTH/6.0f + 5.0/9.0f + 2.0f + 4.4f*j);
            m->setPosY(6.0f+ 6.3f*i);
            counter++;
        }
    }

    for (auto w : m_widgets) {
        w->draw();
    }

    for (auto b : m_buttons) {
        b->draw();
    }

    // if (m_active_button && m_active_button->getText()!="From" && m_active_button->getText()!="To") {
    //     APP()->setScreen("Movie");
    //     APP()->setMovie(((MovieButton*) m_active_button)->getMovie());
    // }
    // m_active_button = nullptr;
}

void BrowseScreen::update() {
    if (APP()->getScreen() != "Browse") return;

    // Get mouse position
    graphics::MouseState ms;
    graphics::getMouseState(ms);

    // Convert mouse cords to canvas cords
    float mx = graphics::windowToCanvasX(ms.cur_pos_x);
    float my = graphics::windowToCanvasY(ms.cur_pos_y);

    // Highlight button
    Button* cur_button = nullptr;
    for (auto w : m_widgets) {
        if (((Slider*)w)->contains(mx, my)) {
            ((Slider*)w)->setHighlight(true);
            cur_button = (Slider*) w;
        } else {
            ((Slider*)w)->setHighlight(false);
        }
    }

    // Activate Button
    if (ms.button_left_pressed && cur_button) {
        m_active_button = cur_button;
        ((Slider*)m_active_button)->setActive(true);
        for (auto b : m_buttons) {
            if (b != m_active_button) {
                if (b->getText() == "From" || b->getText() == "To") {
                    ((Slider*)b)->setActive(false);
                }
            }
        }
    }

    // Move slider
    if (ms.dragging && m_active_button && (m_active_button->getText() == "From" || m_active_button->getText() == "To")) {
        ((Slider*) m_active_button)->slide(mx);
    }

    // Call update on dependent members
    for (auto b : m_buttons) {
        b->update();
    }
    // for (auto w : m_widgets) {
    //     w->update(); //is empty for slider
    // }
}

void BrowseScreen::init() {
    // Intialize Sliders
    Slider* f = new Slider();
    m_widgets.push_front(f);
    m_buttons.push_front(f);
    f->setPosX(CANVAS_WIDTH/6.0f + 5.0/9.0f + 3.0f);
    f->setPosY(0.85f);
    f->setText("From");
    f->init();

    Slider* t = new Slider();
    m_widgets.push_front(t);
    m_buttons.push_front(f);
    t->setPosX(CANVAS_WIDTH/6.0f + 5.0/9.0f + 3.0f);
    t->setPosY(1.85f);
    t->setText("To");
    t->init();
}

BrowseScreen::~BrowseScreen() {
    for (auto w : m_widgets) {
        delete w;
    }
    m_widgets.clear();
    m_buttons.clear();
}

/* Slider */

void Slider::draw() {
    // Draw slider bar
    graphics::Brush br;
    br.outline_width = 1.0f;
    SETCOLOR(br.outline_color, 0.80f, 0.80f, 0.85f);
    SETCOLOR(br.fill_color, 0.80f, 0.80f, 0.85f);
    graphics::drawLine(m_pos[0]-1.1f, m_pos[1], m_pos[0]+1.1f, m_pos[1], br);

    // Draw slider rectangle
    br.outline_opacity = 0.0f;
    SETCOLOR(br.fill_color, 1.0f, 0.54f, .0f);
    graphics::drawRect(m_rect_pos, m_pos[1], 0.1f, 0.4f, br);

    br.fill_opacity = 0.4f * m_highlighted;
    SETCOLOR(br.fill_color, 0.0f, 0.0f, 0.0f);
    graphics::drawRect(m_rect_pos, m_pos[1], 0.1f, 0.4f, br);
}

void Slider::update() {

}

bool Slider::contains(float x, float y) {
    return m_rect_pos-0.05f <= x && x <= m_rect_pos+0.05f && m_pos[1]-0.2f <= y && y <= m_pos[1]+0.2f;
}

void Slider::slide(float x) {
    if (x < m_pos[0]-1.1f) {
        m_rect_pos = m_pos[0]-1.1f;
    } else if (x > m_pos[0]+1.1f) {
        m_rect_pos = m_pos[0]+1.1f;
    } else {
        m_rect_pos = x;
    }
}


/* Info Screen */

void InfoScreen::draw() {
    if (APP()->getScreen() != "Info") return;

    // Draw Pictures
    graphics::Brush br;
    br.fill_opacity = 1.0f;
    br.outline_opacity = 1.0f;
    br.outline_width = 4.0f;
    br.texture = ASSET_PATH + std::string("alex.png");
    SETCOLOR(br.outline_color, 1.0f, 0.54f, .0f);
    graphics::drawRect(m_pos[0] - CANVAS_WIDTH * 5.0f/24.0f, CANVAS_HEIGHT/2.0f - 2.0f, 4.0f, 4.0f, br);
    br.texture = ASSET_PATH + std::string("kate.png");
    graphics::drawRect(m_pos[0] + CANVAS_WIDTH * 5.0f/24.0f, CANVAS_HEIGHT/2.0f - 2.0f, 4.0f, 4.0f, br);

    // Draw contact info
    br.outline_opacity = 0.0f;
    graphics::setFont(ASSET_PATH + std::string("MontserratMedium.ttf"));
    graphics::drawText(m_pos[0] - CANVAS_WIDTH * 5.0f/24.0f - 2.1f, CANVAS_HEIGHT/2.0f + 1.5f, 0.45f, "Alex    Papadopoulos", br);
    graphics::drawText(m_pos[0] + CANVAS_WIDTH * 5.0f/24.0f - 2.0f, CANVAS_HEIGHT/2.0f + 1.5f, 0.45f, "Katerina    Mantaraki", br);

    // Draw Social Media
    br.texture = ASSET_PATH + std::string("linkedin.png");
    graphics::drawRect(m_pos[0] - CANVAS_WIDTH * 5.0f/24.0f - 2.55f, CANVAS_HEIGHT/2.0f + 3.0f, 0.6f, 0.6f, br);
    graphics::drawRect(m_pos[0] + CANVAS_WIDTH * 5.0f/24.0f - 2.45f, CANVAS_HEIGHT/2.0f + 3.0f, 0.6f, 0.6f, br);

    graphics::drawText(m_pos[0] - CANVAS_WIDTH * 5.0f/24.0f - 2.1f, CANVAS_HEIGHT/2.0f + 3.15f, 0.35f, "linkedin.com/in/alexiospap", br);
    graphics::drawText(m_pos[0] + CANVAS_WIDTH * 5.0f/24.0f - 2.0f, CANVAS_HEIGHT/2.0f + 3.15f, 0.35f, "linkedin.com/in/katerinamant", br);

    br.texture = ASSET_PATH + std::string("github.png");
    graphics::drawRect(m_pos[0] - CANVAS_WIDTH * 5.0f/24.0f - 2.55f, CANVAS_HEIGHT/2.0f + 4.0f, 0.75f, 0.75f, br);
    graphics::drawRect(m_pos[0] + CANVAS_WIDTH * 5.0f/24.0f - 2.45f, CANVAS_HEIGHT/2.0f + 4.0f, 0.75f, 0.75f, br);

    graphics::drawText(m_pos[0] - CANVAS_WIDTH * 5.0f/24.0f - 2.1f, CANVAS_HEIGHT/2.0f + 4.15f, 0.35f, "github.com/alexisthedev", br);
    graphics::drawText(m_pos[0] + CANVAS_WIDTH * 5.0f/24.0f - 2.0f, CANVAS_HEIGHT/2.0f + 4.15f, 0.35f, "github.com/katerinamant", br);
}

void InfoScreen::update() {
    if (APP()->getScreen() != "Info") return;
}

/* Movie Screen */

void MovieScreen::draw() {
    if (APP()->getScreen() != "Movie") return;

    Movie* m = APP()->getMovie();

    // Draw movie details
    graphics::Brush br;
    br.outline_opacity = 0.0f;

    // Add gradient
    SETCOLOR(br.fill_secondary_color, 0.03f, 0.04f, 0.12f);
    br.fill_secondary_opacity = 1.0f;
    br.gradient = true;
    br.gradient_dir_u = 0.0f;
    br.gradient_dir_v = 1.0f;

    // Draw movie banner
    br.texture = m->getBanner();
    graphics::drawRect(m_pos[0], CANVAS_HEIGHT/5.0f, CANVAS_WIDTH, 2.0f*CANVAS_HEIGHT/5.0f, br);

    br.fill_secondary_opacity = 0.0f;
    br.gradient = false;

    // Draw movie poster
    br.texture = m->getPoster();
    br.outline_opacity = 1.0f;
    br.outline_width = 4.0f;
    SETCOLOR(br.outline_color, 1.0f, 1.0f, 1.0f);
    graphics::drawRect(CANVAS_WIDTH/8.0f, 2.0f*CANVAS_HEIGHT/5.0f, 4.375f, 6.25f, br);

    // Draw movie information
    br.texture = "";
    br.outline_opacity = 0.0f;
    SETCOLOR(br.fill_color, 1.0f, 1.0f, 1.0f);
    graphics::drawText(CANVAS_WIDTH/8.0f + 2.8f, 2.0f*CANVAS_HEIGHT/5.0f + 1.25f, 0.8f, m->getTitle() + " (" + std::to_string(m->getYear())+")", br);

    SETCOLOR(br.fill_color, 0.80f, 0.80f, 0.85f);
    graphics::drawText(CANVAS_WIDTH/8.0f + 2.8f, 2.0f*CANVAS_HEIGHT/5.0f + 2.0f, 0.4f, "Directed  by:  " + m->getDirectors(), br);
    graphics::drawText(CANVAS_WIDTH/8.0f + 2.8f, 2.0f*CANVAS_HEIGHT/5.0f + 2.75f, 0.4f, "Starring:  " + m->getActors(), br);
    graphics::drawText(CANVAS_WIDTH/8.0f - 2.18525f, 2.0f*CANVAS_HEIGHT/5.0f + 3.875f, 0.4f, "Genre:  " + m->getGenre(), br);
    graphics::drawText(CANVAS_WIDTH/8.0f - 2.18525f, 2.0f*CANVAS_HEIGHT/5.0f + 4.6f, 0.4f, m->getDesc(), br);

    // Draw movie scenes
    br.texture = m->getScene1();
    graphics::drawRect(m_pos[0] - 7.0f, CANVAS_HEIGHT - 2.5f, 6.0f, 4.0f, br);
    br.texture = m->getScene2();
    graphics::drawRect(m_pos[0] + 7.0f, CANVAS_HEIGHT - 2.5f, 6.0f, 4.0f, br);

    for (auto b : m_buttons) {
        b->draw();
    }
}

void MovieScreen::update() {
    // Get mouse position
    graphics::MouseState ms;
    graphics::getMouseState(ms);

    // Convert mouse cords to canvas cords
    float mx = graphics::windowToCanvasX(ms.cur_pos_x);
    float my = graphics::windowToCanvasY(ms.cur_pos_y);

    // Highlight button
    SlideshowButton* cur_button = nullptr;
    for (auto b : m_buttons) {
        if (b->contains(mx, my)) {
            b->setHighlight(true);
            cur_button = b;
        } else {
            b->setHighlight(false);
        }
    }

    // Change to next or prev slide
    if (ms.button_left_pressed && cur_button) {
        if (cur_button->getText() == "Back") {
            APP()->setScreen(APP()->getPrevScreen());
            APP()->setMovie(nullptr);
        }
    }

    // Call update on dependent members: buttons
    for (auto b : m_buttons) {
        b->update();
    }
}

void MovieScreen::init() {
    // We will be reusing the slideshow button for the "X" icon of this screen
    // Yes we know that the name doesn't make sense
    // No we will not change it
    SlideshowButton* b = new SlideshowButton();
    m_buttons.push_back(b);
    b->setText("Back");
    b->setIcon(ASSET_PATH + std::string("back.png"));
    b->setPosX(1.0f);
    b->setPosY(1.0f);
}

MovieScreen::~MovieScreen() {
    for (auto b : m_buttons) {
        delete b;
    }
    m_buttons.clear();
}
