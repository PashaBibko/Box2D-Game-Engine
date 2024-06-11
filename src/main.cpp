#include <util/util.h>
#include <engine/engine.h>

class CustomController : public EngineController
{
	private:
		std::shared_ptr<Entity> entity;

	public:
		void onInit() override
		{
			entity = GraphicEntity::create(Vec2{100, 100}, Vec2{100, 100});
		}

		void onRender() override
		{
			entity->render();
		}
};

int main()
{
	Engine instance(Vec2{ 800, 600 }, std::make_unique<CustomController>());

	BASIC_LOOP(instance)

	return 0;
}