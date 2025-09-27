2025-09-07
==========

Idea behind a blog format is to be able to write down ideas as fast as possible without orginizing it to prevent forgetting.

Current problem is how to do the description of infocells.

Set
---

In case of a SET(cell, key, value) the description can be that it make it possible to do a GET(cell, key) and the value of GET will be value.

So I can create an index struct 

let m_cell is an InfoCell input
let m_key is an InfoCell input
let m_value is an InfoCell input

SAME(lhs: GET(cell: m_cell, key: m_key), rhs: m_value)

Get
---

In case of GET(cell, key) the description can be that this().value will be the same as cell.key
let m_cell is an InfoCell input
let m_key is an InfoCell input
let m_value is an InfoCell output

GET(cell: m_cell,  key: m_key)
SAME(lhs: GET(cell: m_cell, key: m_key), rhs: m_value)

But what about EQUAL and SAME?

Request (or maybe expectations?)
-------

So a I want to write a request where only the end result specified. For example

let the color of the LED is green.

The LED is in kb.output.led. The green is in kb.colors.green.

So with ASTs I can write

SAME(GET(_(kb.output.led), _(kb.ids.color)), _(kb.colors.green))


2025-09-14
==========

Ok so I went to a direction of sorting out the for loop as I want see how can handle multiple values in a container.

The Rust way:

pub trait IntoIterator {
    type Item;
    type IntoIter: Iterator<Item = Self::Item>;

    // Required method
    fn into_iter(self) -> Self::IntoIter;
}

pub trait Iterator {
    type Item;
    ...
}

impl<T, A: Allocator> IntoIterator for LinkedList<T, A> {
    type Item = T;
    type IntoIter = Iter<T>;

    fn into_iter(self) -> Iter<T> {
        self.iter()
    }
}

impl<T, A: Allocator> LinkedList<T, A> {

    pub fn iter(&self) -> Iter<'_, T> {
        Iter { head: self.head, tail: self.tail, len: self.len, marker: PhantomData }
    }

}

pub struct Iter<T> {
    head: Node<T>,
    tail: Node<T>,
    len: usize
}

impl<'a, T> Iterator for Iter<'a, T> {
    type Item = &'a T;

    #[inline]
    fn next(&mut self) -> Option<&'a T> {
      ...
    }

}

My idea

    List<T> has trait Iterable

    so

	var list = List<int>(); // for a list instance
	var iteratorForList = cast<Iterable>(list).iterator(); // we can have an iterator


	trait Iterable {
        type Iterator : Iterator;

        fn iterator() -> Self::Iterator;
    }

    trait Iterator
    {
        type ValueType;

        bool isEmpty();
        void setFirstValue();
        Self::ValueType getCurrentValue();
        bool hasNextValue();
        void setNextValue();
    }

2025-09-14
==========

Ok, this should work. A trait doesn't contains member definitions.

struct Map {
    traits Container; // this will be registered when impl<T> Container for List<T>
}

2025-09-23
==========

Ok, the trait concept just do the trick. I can go back to sort out the InfoTool concept. So the ide here is that an InfoTool is a tool that knows it's own effect.
The effect is written down by description. So the problem how to do this descrition. If I have a description language I can write down the effect that I want to achive
and now I can find InfoTools that do these effects.

I playing with the idea to create a Storage trait. And also I need a description of the InfoCell somehow.

The InfoCell

- can store slots
- contains an address which is an opaque type


The Slot can contains

- a name which is an InfoCellAddress
- a value which is an InfoCellAddress

```
trait Storage {
    type Item;

    fn store(item: Self::Item)
       description {
          effect {
             after(self.contains(item) == true);
          }
       }

    fn remove(item: Self::Item)
       description {
          effect {
             after(self.contains(item) == false);
          }
       }

    fn isEmpty() -> bool
       description {
           // no state change so no effect block
           match (return) {
             case true:
              var item: Self::Item;
              self.contains(item) == false;
             case false:
              var item: Self::Item;
              self.contains(item) == true;
           }
       }

    fn contains(item: Self::Item) -> bool
       description {
           // no state change so no effect block
           if (return == true) {
              before(store(item));
           } else {
              before(remove(item)) or never(store(item));
           }
       }

    fn get() -> Self::Item {
    }
}

trait AssociativeStorage {
    type Key;
    type Value;

    fn store(key: Self::Key, value: Self::Value)
       description {
          effect {
             after(self.hasKey(key) == true);
             after(self.getValue(key) == value);
          }
       }

    fn remove(key: Self::Key)
       description {
          effect {
             after(self.hasKey(key) == false);
          }
       }

    fn isEmpty() -> bool
       description {
           // no state change so no effect block
           if (return == true) {
              var item: Self::Item; // so any item
              self.contains(item) == false;
           }
       }

    fn hasKey(key: Self::Key) -> bool
       description {
           // no state change so no effect block
           var value: Unknown;
           if (return == true) {
              before(store(key, value));
           } else {
              before(remove(key)) or never(store(key, value));
           }
       }

    fn getValue(key: Self::Key) -> Self::Value
       description {
          if (hasKey(key) == false) {
             #error
          }
          var value: Unknown;
          before(store(key, value));

          return == value;
       }
}

```

The trait Storage is implemented for Slot and InfoCell also.


2025-09-25
==========

1. still not fully satisfied with the descriptions. I am thinking on the microcode way, so creating a descriptor thing just
   to explcitliy stating the what cell and what slot changed from this to that. So this will be some kind of transition language.

1. Maybe I should use the "return" or "result" names instead of "value". There is name collision with SET. There we have cell, name (role, key), value.
   But that value is not a result value.

```
concept Storage {
    type Item;
    enum State { Empty, Store }; // when capacity is 1

    premise store(item: Self::Item) {
        self.contains(item) == true;
        self.get() == item;
        reason(self.store(item));
    }

    premise missing(item: Self::Item) {
        self.contains(item) == false;
        reason(self.remove(item) or never(self.store(item)));
    }

    premise empty() {
        any_var item;
        self.contains(item) == false;
        reason(self.remove(item) or never(self.store(item)));
    }

    fn store(item: Self::Item)
       description {
          consequence(premise_(store(item)));
       }

    fn remove(item: Self::Item)
       description {
          consequence(premise_(empty()) and premise_(missing(item)));
       }

    fn getState() -> State
       description {
           var item = return_();
           match item {
             State::Empty => premise_(empty());
             State::Store => premise_(store(item));
           }
       }

    fn contains(item: Self::Item) -> bool
       description {
           match return_() {
               true => premise_(store(item));
               false => premise_(missing(item));
           }
       }

    fn get() -> Self::Item {
       description {
           if (premise_(empty())) {
              #error
           }
           var item;
           premise_(store(item));
           return_() == item;
       }
    }
}
```


2025-09-27
==========

```
concept Storage {
    type Item;
    enum State { Empty, Store }; // when capacity is 1

    statement store(item: Self::Item) {
        self.contains(item) == true;
        self.get() == item;
        reason(self.store(item));
    }

    statement missing(item: Self::Item) {
        self.contains(item) == false;
        reason(self.remove(item) or never(self.store(item)));
    }

    statement empty() {
        any_var item;
        self.contains(item) == false;
        reason(self.remove(item) or never(self.store(item)));
    }

    fn store(item: Self::Item)
       description {
          consequence(statement_(store(item)));
       }

    fn remove(item: Self::Item)
       description {
          consequence(statement_(empty()) and statement_(missing(item)));
       }

    fn getState() -> State
       description {
           var item = return_();
           match item {
             State::Empty => statement_(empty());
             State::Store => statement_(store(item));
           }
       }

    fn contains(item: Self::Item) -> bool
       description {
           match return_() {
               true => statement_(store(item));
               false => statement_(missing(item));
           }
       }

    fn get() -> Self::Item {
       description {
           if (statement_(empty())) {
              #error
           }
           var item;
           statement_(store(item));
           return_() == item;
       }
    }
}
```