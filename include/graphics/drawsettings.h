//
// Created by marten on 18-06-20.
//

#ifndef DRAWSETTINGS_H
#define DRAWSETTINGS_H

namespace glpp {

	class Shader;
	class Texture;

	/**
 * Defines how a Drawable will be rendered
 */
	struct DrawSettings {
		DrawSettings();
		explicit DrawSettings(Shader* shader);
		explicit DrawSettings(Texture* texture);
		DrawSettings(Texture* texture, Shader* shader);

		Shader*             shader;
		Texture*            texture;
		static DrawSettings defaultSettings;
	};

}

#endif //DRAWSETTINGS_H
