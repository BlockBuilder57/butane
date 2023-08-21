#pragma once

#include <core/Types.hpp>
#include <vector>

namespace engine::core::scene {

	template<class T>
	struct SGNodeMixin {
		using SmartType = std::unique_ptr<T>;
		using ListType = std::vector<SmartType>;

		template<class TOther = T, class... Args>
		inline SmartType& AddParent(const std::string& name, Args&&... args) {
			static_assert(std::is_base_of_v<T, TOther>, "TOther must inherit from root scene graph type");
			auto& back = children.template emplace_back(std::make_unique<TOther>(std::forward(args)...));

			// Set up the object.
			back->SetName(name);
			back->parent = static_cast<T*>(this);
			return back;
		}

		[[nodiscard]] const std::string_view GetName() const {
			return name;
		}

		void SetName(const std::string& newName) {
			name = newName;
		}

		template<class U>
		inline bool Is() {
			return dynamic_cast<U*>(static_cast<T*>(this)) != nullptr;
		}

		template<class U>
		inline U* As() {
			assert(Is<U>());
			return dynamic_cast<U*>(static_cast<T*>(this));
		}

		// TODO: searches.. should they also be template<class U = T> to allow constraints?
		// 		(breadth first & depth searches)
		//		Also, it would be a good idea to be able to compute/query node depth.
		//		Maybe API to query/set parent (make sure the parent is inside the tree?)

		template<class Callback>
		void Traverse(Callback&& cb) {
			auto* head = parent;

			if(this->parent == nullptr) {
				// We are already at the head node of the graph.
				head = static_cast<T*>(this);
			} else {
				// Go back to the head node of the graph, so we can traverse
				// forwards.
				while(true) {
					auto* last_parent = head->parent;

					if(last_parent == nullptr)
						break;

					head = last_parent;
				}
			}

			cb(head, 0);

			// Traverse into the tree.
			for(auto& child : head->children) {
				auto depth = 1;
				cb(child.get(), depth);

				// traverse into children of child node, if any exist
				if(!child->children.empty()) {
					depth++;
					for(auto& child2 : child->children) {
						cb(child2.get(), depth);
					}
				}
			}
		}

	   private:
		std::string name{};

		T* parent{nullptr}; // who created me
		ListType children;  // adjacent to me.
	};
}